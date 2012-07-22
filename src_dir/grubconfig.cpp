#include "grubconfig.h"

std::string getProxifiedScriptName(std::string const& proxyScriptPath){ //return value: name of proxy or emtpy string if it isn't a proxy
	FILE* proxy_fp = fopen(proxyScriptPath.c_str(), "r");
	int c;
	//skip first line
	while ((c = fgetc(proxy_fp)) != EOF){
		if (c == '\n')
			break;
	}
	//compare the line start
	std::string textBefore = "#THIS IS A GRUB PROXY SCRIPT FOR ";
	bool match_error = false;
	for (int i = 0; i < textBefore.length() && (c = fgetc(proxy_fp)) != EOF; i++){
		if (c != textBefore[i]){
			match_error = true; //It's not a proxy.
			break;
		}
	}
	
	std::string proxyfied_script = "";
	if (!match_error){
		//read the script name (ends by line break)
		while ((c = fgetc(proxy_fp)) != EOF){
			if (c == '\n')
				break;
			else
				proxyfied_script += c;
		}
	}
	
	fclose(proxy_fp);
	return proxyfied_script;
}

ToplevelScript::ToplevelScript(std::string name, std::string proxyfiedScriptName, int permissions)
	: name(name), proxyfiedScriptName(proxyfiedScriptName), permissions(permissions)
{
	this->isProxy = proxyfiedScriptName != "";
	std::string strpart_index = name.substr(0,2);
	this->index = ((strpart_index[0] - '0') * 10) + (strpart_index[1] - '0'); //converting the index to int
}

std::string ToplevelScript::getBasename(){
	if (this->proxyfiedScriptName != "")
		return this->proxyfiedScriptName;
	else
		return this->name.substr(3);
}

ToplevelScript::ToplevelScript(){}

bool ToplevelScript::isExecutable() const {
	return permissions & 0111;
}

void ToplevelScript::set_executable(bool is_executable){
	if (is_executable)
		permissions |= 0111;
	else
		permissions &= ~0111;
}

bool compare_scripts(ToplevelScript const& a, ToplevelScript const& b){
	return a.index < b.index;
}

GrubConfig::GrubConfig()
	: connectedUI(NULL), progress(0), cancelThreadsRequested(false), mkconfigProc(NULL)
{}

void GrubConfig::send_new_load_progress(double newProgress){
	if (connectedUI != NULL){
		this->progress = newProgress;
		connectedUI->event_load_progress_changed();
	}
	else {
		std::cerr << "Error: cannot show updated load progress - no UI connected!" << std::endl;
	}
}

void GrubConfig::send_new_save_progress(double newProgress){
	if (connectedUI != NULL){
		this->progress = newProgress;
		connectedUI->event_save_progress_changed();
	}
	else {
		std::cerr << "Error: cannot show updated save progress - no UI connected!" << std::endl;
	}
}

void GrubConfig::cancelThreads(){
	cancelThreadsRequested = true;
}

void GrubConfig::load(){
	this->send_new_load_progress(0);
	int toplevelScriptCount = 0; //does equal with this->size() in some cases (disabled scripts will be counted too)
	
	//reading the current configuration
	DIR* hGrubCfgDir = opendir("/etc/grub.d");
	
	if (!hGrubCfgDir){
		if (connectedUI){
			this->message = gettext("/etc/grub.d not found. Is grub2 installed?");
			connectedUI->event_thread_died();
		}
		return; //cancel this thread
	}

	struct dirent *entry;
	struct stat fileProperties;
	while (entry = readdir(hGrubCfgDir)){
		stat((std::string("/etc/grub.d/")+entry->d_name).c_str(), &fileProperties);
		if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
			if (entry->d_name[2] == '_' && entry->d_name[0] != '0'){ //check whether it's an script (they should be named XX_scriptname)… und block header scripts (they use a leading 0)
				this->push_back(ToplevelScript(entry->d_name, getProxifiedScriptName((std::string("/etc/grub.d/")+entry->d_name).c_str()), fileProperties.st_mode & ~S_IFMT));
				toplevelScriptCount++;
			}
		}
	}
	closedir(hGrubCfgDir);

	this->sort(compare_scripts); //sorting by name … as the update-grub script would do too

	//std::cout << "Anzahl der Skripte: " << this->realScripts.size() << std::endl;

	this->send_new_load_progress(0.05);

	//link proxified scripts and make them all executable
	DIR* hPsScriptDir = opendir("/etc/grub.d/proxifiedScripts");

	for (std::list<ToplevelScript>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->proxyfiedScriptName == ""){
			if (!iter->isExecutable()){
				std::cout << "setting executable bit for " << iter->name << std::endl;
				int success = chmod(("/etc/grub.d/"+iter->name).c_str(), 0755);
				if (success != 0)
					std::cerr << "Berechtigungen eines Scriptes konnten nicht geändert werden!" << std::endl;
			}
		}
		else {
			int success = chmod(("/etc/grub.d/"+iter->name).c_str(), 0644);
			if (success != 0)
				std::cerr << "Berechtigungen eines Proxies konnten nicht geändert werden!" << std::endl;
		
			std::cout << "adding link for " << iter->proxyfiedScriptName << std::endl;
			if (hPsScriptDir)
				int link_successful_created = symlink(("/etc/grub.d/proxifiedScripts/"+iter->proxyfiedScriptName).c_str(), ("/etc/grub.d/LS_"+iter->proxyfiedScriptName).c_str());
		}
	}
	
	//link all remaining (unused) scripts from proxifiedScripts
	if (hPsScriptDir){
		while (entry = readdir(hPsScriptDir)){
			stat((std::string("/etc/grub.d/proxifiedScripts/")+entry->d_name).c_str(), &fileProperties);
			if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
				int success = symlink((std::string("/etc/grub.d/proxifiedScripts/")+entry->d_name).c_str(), (std::string("/etc/grub.d/LS_")+entry->d_name).c_str());
				if (success == 0)
					toplevelScriptCount++;
			}
		}
		closedir(hPsScriptDir);
	}

	this->send_new_load_progress(0.1);

	//execute scripts
	mkconfigProc = popen("grub-mkconfig", "r");
	GrubConfRow row;
	std::string scriptName;
	int i = 0;
	while (!cancelThreadsRequested && (row = readGrubConfRow(mkconfigProc))){
		if (row.text.substr(0,22) == "### BEGIN /etc/grub.d/" && row.text.substr(row.text.length()-4,4) == " ###"){
			scriptName = row.text.substr(22, row.text.length()-26);
			if (scriptName[0] != '0'){ //ignore header scripts (00_header, 05_debian_theme)
				this->send_new_load_progress(0.1 + (0.7 / toplevelScriptCount * ++i));
				this->realScripts[scriptName.substr(3)]; //make sure that empty scripts will be added to realScripts
			}
		}
		else if (row.text.substr(0, 10) == "menuentry ") {
			Entry grubEntry = readGrubEntry(mkconfigProc, row);
			if (scriptName.substr(0,3) == "LS_"){
				std::string proxyfiedScriptName = scriptName.substr(3);
				for (std::list<ToplevelScript>::iterator confIter = this->begin(); confIter != this->end(); confIter++){
					if (confIter->proxyfiedScriptName == proxyfiedScriptName){
						confIter->entries.push_back(grubEntry);
					}
				}
			}
			else {
				for (std::list<ToplevelScript>::iterator confIter = this->begin(); confIter != this->end(); confIter++){
					if (confIter->name == scriptName){
						confIter->entries.push_back(grubEntry);
						break;
					}
				}
			}
			this->realScripts[scriptName.substr(3)].push_back(grubEntry);
			
			this->send_new_load_progress(0.1 + (0.7 / toplevelScriptCount * i));
		}
	}

	int success = pclose(mkconfigProc);
	if (success != 0 && !cancelThreadsRequested){
		if (connectedUI){
			this->message = gettext("grub-mkconfig couldn't be executed successfully. You must run this as root!");
			connectedUI->event_thread_died();
		}
		return; //cancel this thread
	}

	this->send_new_load_progress(0.9);
	
	mkconfigProc = NULL;
	
	//restore configuration
	for (std::list<ToplevelScript>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->proxyfiedScriptName == ""){
			int success = chmod(("/etc/grub.d/"+iter->name).c_str(), iter->permissions);
			if (success != 0)
				std::cerr << "Berechtigungen eines Scriptes konnten nicht geändert werden!" << std::endl;
		}
		else {
			int success = chmod(("/etc/grub.d/"+iter->name).c_str(), iter->permissions);
			if (success != 0)
				std::cerr << "Berechtigungen eines Proxies konnten nicht geändert werden!" << std::endl;
		
			unlink(("/etc/grub.d/LS_"+iter->proxyfiedScriptName).c_str());
		}
	}

	//remove all links of remaining (unused) proxifiedScripts
	hPsScriptDir = opendir("/etc/grub.d/proxifiedScripts");
	if (hPsScriptDir){
		while (entry = readdir(hPsScriptDir)){
			stat((std::string("/etc/grub.d/proxifiedScripts/")+entry->d_name).c_str(), &fileProperties);
			if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
				unlink((std::string("/etc/grub.d/LS_")+entry->d_name).c_str()); //should not be successfull in every case
			}
		}
		closedir(hPsScriptDir);
	}

	this->send_new_load_progress(0.97);

	//execute proxies

	for (std::list<ToplevelScript>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->proxyfiedScriptName != ""){
			FILE* proxyFile = fopen(("/etc/grub.d/"+iter->name).c_str(), "r");
			ProxyscriptData proxyData = parseProxyScript(proxyFile);
			fclose(proxyFile);
			if (proxyData.ruleString != ""){
				iter->entries = executeListRules(iter->entries, readRuleString(proxyData.ruleString.c_str()));
			}
		}
	}
	this->send_new_load_progress(1);
}

std::string GrubConfig::getMessage() const {
	return message;
}

GrubConfig::iterator GrubConfig::getIterByPointer(ToplevelScript* ptr){
	GrubConfig::iterator iter = this->begin();
	while (iter != this->end() && &*iter != ptr){
		iter++;
	}
	return iter;
}

void GrubConfig::generateProxy(FILE* proxyFile, ToplevelScript* script){
	fputs("#!/bin/sh\n#THIS IS A GRUB PROXY SCRIPT FOR ", proxyFile);
	fputs(script->proxyfiedScriptName.c_str(), proxyFile);
	fputs("\n", proxyFile);
	fputs("/etc/grub.d/proxifiedScripts/", proxyFile);
	fputs(script->proxyfiedScriptName.c_str(), proxyFile);
	fputs(" | /etc/grub.d/bin/grubcfg_proxy \"", proxyFile);
	int i = 0;
	for (std::list<Entry>::iterator entryIter = script->entries.begin(); entryIter != script->entries.end(); entryIter++){
		if (i == script->entries.other_entries_pos){
			fputs(script->entries.other_entries_visible ? "+*\n" : "-*\n", proxyFile);
		}
		fputs(entryIter->disabled ? "-'" : "+'", proxyFile);
		fputs(entryIter->name.c_str(), proxyFile);
		fputs("'", proxyFile);
		if (entryIter->outputName != entryIter->name){
			fputs(" as '", proxyFile);
			fputs(entryIter->outputName.c_str(), proxyFile);
			fputs("'", proxyFile);
		}
		fputs("\n", proxyFile);
		i++;
	}
	fputs("\"", proxyFile);
}

void GrubConfig::save(){
	send_new_save_progress(0);
	
	std::list<std::string> unmodifiedScripts;
	
	{
		struct dirent *entry;
		struct stat fileProperties;
		DIR* hScriptDir = opendir("/etc/grub.d");
		while (entry = readdir(hScriptDir)){
			if (entry->d_name[2] == '_' && entry->d_name[0] != '0'){ //only script, without ignore header scripts
				stat((std::string("/etc/grub.d/")+entry->d_name).c_str(), &fileProperties);
				if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
					unmodifiedScripts.push_back(entry->d_name);
				}
			}
		}
		closedir(hScriptDir);
	}
	
	int mkdir_result = mkdir("/etc/grub.d/proxifiedScripts", 0755); //create this directory if it doesn't allready exist
	
	int proxyCount = 0;
	for (std::list<ToplevelScript>::iterator iter = this->begin(); iter != this->end(); iter++){
		unmodifiedScripts.remove(iter->name);
		std::ostringstream newName;
		newName << iter->index << "_" << iter->getBasename();
		if (iter->isProxy)
			newName << "_proxy";
		
		bool file_exists = false;
		FILE* f = fopen(("/etc/grub.d/"+iter->name).c_str(), "r");
		if (f){
			file_exists = true;
			fclose(f);
		}
		else
			std::cout << "file doesn't exist: " << iter->name << std::endl;
		
		std::cout << "new name: " << newName.str() << std::endl;
		if (iter->isProxy && iter->proxyfiedScriptName == ""){
			iter->proxyfiedScriptName = iter->getBasename();
			int res = 0;
			if (file_exists){
				chmod(("/etc/grub.d/"+iter->name).c_str(), 0755);
				res = rename(("/etc/grub.d/"+iter->name).c_str(), ("/etc/grub.d/proxifiedScripts/"+iter->proxyfiedScriptName).c_str());
			}
			if (res == 0){
				iter->name = newName.str();
				FILE* proxyFile = fopen(("/etc/grub.d/"+iter->name).c_str(), "w");
				generateProxy(proxyFile, &*iter);
				proxyCount++;
				fclose(proxyFile);
			}
			else
				std::cerr << "could not move the script file!" << std::endl;
		}
		else {
			if (iter->name != newName.str()){
				if (file_exists){
					rename(("/etc/grub.d/"+iter->name).c_str(), ("/etc/grub.d/"+newName.str()).c_str());
					std::cout << "renaming " << iter->name << " to " << newName.str() << std::endl;
				}
				else {
					rename(("/etc/grub.d/proxifiedScripts/"+iter->getBasename()).c_str(), ("/etc/grub.d/"+newName.str()).c_str());
				}
				iter->name = newName.str();
			}
			if (iter->isProxy && iter->proxyfiedScriptName != ""){
				FILE* proxyFile = fopen(("/etc/grub.d/"+iter->name).c_str(), "w");
				generateProxy(proxyFile, &*iter);
				proxyCount++;
				fclose(proxyFile);
			}
		}
		std::cout << "setting permisssions for " << iter->name << ": " << iter->permissions << std::endl;
		chmod(("/etc/grub.d/"+iter->name).c_str(), iter->permissions);
	}
	send_new_save_progress(0.2);
	
	for (std::list<std::string>::iterator iter = unmodifiedScripts.begin(); iter != unmodifiedScripts.end(); iter++){
		if (getProxifiedScriptName("/etc/grub.d/"+(*iter)) == ""){
			chmod(("/etc/grub.d/"+(*iter)).c_str(), 0755);
			rename(("/etc/grub.d/"+(*iter)).c_str(), ("/etc/grub.d/proxifiedScripts/"+iter->substr(3)).c_str());
		}
		else
			unlink(("/etc/grub.d/"+(*iter)).c_str());
	}
	
	//remove "proxifiedScripts" dir, if empty
	
	{
		int proxifiedScriptCount = 0;
		struct dirent *entry;
		struct stat fileProperties;
		DIR* hScriptDir = opendir("/etc/grub.d/proxifiedScripts");
		while (entry = readdir(hScriptDir)){
			if (std::string(entry->d_name) != "." && std::string(entry->d_name) != ".."){
				proxifiedScriptCount++;
			}
		}
		closedir(hScriptDir);
		
		if (proxifiedScriptCount == 0)
			rmdir("/etc/grub.d/proxifiedScripts");
	}
	
	//add or remove proxy binary
	
	FILE* proxyBin = fopen("/etc/grub.d/bin/grubcfg_proxy", "r");
	bool proxybin_exists = proxyBin != NULL;
	if (proxyBin)
		fclose(proxyBin);
	
	if (proxyCount != 0 && !proxybin_exists){
		std::cout << "proxyCount: " << proxyCount << std::endl;
		//copy proxy
		int bin_mk_success = mkdir("/etc/grub.d/bin", 0755);
		if (bin_mk_success == EEXIST || bin_mk_success == 0){
			FILE* proxyBinSource = fopen((std::string(LIBDIR)+"/grubcfg-proxy").c_str(), "r");
			
			if (proxyBinSource){
				FILE* proxyBinTarget = fopen("/etc/grub.d/bin/grubcfg_proxy", "w");
				if (proxyBinTarget){
					int c;
					while ((c = fgetc(proxyBinSource)) != EOF){
						fputc(c, proxyBinTarget);
					}
					fclose(proxyBinTarget);
					chmod("/etc/grub.d/bin/grubcfg_proxy", 0755);
				}
				fclose(proxyBinSource);
			}
			
		}
	}
	else if (proxyCount == 0 && proxybin_exists){
		//the following commands are only cleanup… no problem, when they fail
		unlink("/etc/grub.d/bin/grubcfg_proxy");
		rmdir("/etc/grub.d/bin");
	}
	
	//run update-grub
	FILE* saveProc = popen("update-grub 2>&1", "r");
	int c;
	std::string row = "";
	while ((c = fgetc(saveProc)) != EOF){
		if (c == '\n'){
			send_new_save_progress(0.5); //a gui should use pulse() instead of set_fraction
			row = "";
		}
		else
			row += char(c);
		std::cerr << char(c); //print messages (for debugging purposes)
	}
	pclose(saveProc);
	
	send_new_save_progress(1);
}

void GrubConfig::increaseScriptPos(ToplevelScript* script){
	short int i = 10;
	bool scriptToMove_found = false;
	for (std::list<ToplevelScript>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (&*iter != script){
			iter->index = i++;
			if (scriptToMove_found){
				script->index = i++;
				scriptToMove_found = false;
			}
		}
		else
			scriptToMove_found = true;
	}
	this->sort(compare_scripts);
}

void GrubConfig::renumerate(){
	increaseScriptPos(NULL);
}

void GrubConfig::reset(){
	this->clear();
	this->realScripts.clear();
}

void GrubConfig::connectUI(GrubConfUI& ui){
	connectedUI = &ui;
}

double GrubConfig::getProgress() const {
	return progress;
}

void GrubConfig::copyScriptFromRepository(std::string const& name){
	EntryList selectedList = this->realScripts[name];
	bool sameScriptFound = false;
	for (GrubConfig::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->getBasename() == name){
			if (!iter->isProxy){
				iter->isProxy = true;
				iter->entries.other_entries_pos = 0;
				iter->entries.other_entries_visible = true;
			}
			sameScriptFound = true;
		}
	}
	this->push_back(ToplevelScript("99_"+name, "", 99));
	this->back().entries = selectedList;
	this->back().permissions = 0755;

	if (sameScriptFound){
		this->back().isProxy = true;
		this->back().entries.other_entries_pos = 0;
		this->back().entries.other_entries_visible = true;
	}
	
	this->renumerate();
}
