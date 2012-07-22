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
	: connectedUI(NULL), progress(0), cancelThreadsRequested(false), mkconfigProc(NULL), burgMode(false), config_has_been_different_on_startup_but_unsaved(false), error_proxy_not_found(false)
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
	DIR* hGrubCfgDir = opendir(this->cfg_dir.c_str());
	
	if (!hGrubCfgDir){
		if (connectedUI){
			this->message = this->cfg_dir+gettext(" not found. Is grub2 installed?");
			connectedUI->event_thread_died();
		}
		return; //cancel this thread
	}

	struct dirent *entry;
	struct stat fileProperties;
	while (entry = readdir(hGrubCfgDir)){
		stat((this->cfg_dir+"/"+entry->d_name).c_str(), &fileProperties);
		if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
			if (entry->d_name[2] == '_' && entry->d_name[0] != '0'){ //check whether it's an script (they should be named XX_scriptname)… und block header scripts (they use a leading 0)
				this->push_back(ToplevelScript(entry->d_name, getProxifiedScriptName((this->cfg_dir+"/"+entry->d_name).c_str()), fileProperties.st_mode & ~S_IFMT));
				toplevelScriptCount++;
			}
		}
	}
	closedir(hGrubCfgDir);

	this->sort(compare_scripts); //sorting by name … as the update-grub script would do too

	//std::cout << "Anzahl der Skripte: " << this->realScripts.size() << std::endl;

	this->send_new_load_progress(0.05);

	//link proxified scripts and make them all executable
	DIR* hPsScriptDir = opendir((this->cfg_dir+"/proxifiedScripts").c_str());

	for (std::list<ToplevelScript>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->proxyfiedScriptName == ""){
			if (!iter->isExecutable()){
				std::cout << "setting executable bit for " << iter->name << std::endl;
				int success = chmod((this->cfg_dir+"/"+iter->name).c_str(), 0755);
				if (success != 0)
					std::cerr << "couln't manipulate script permissions!" << std::endl;
			}
		}
		else {
			int success = chmod((this->cfg_dir+"/"+iter->name).c_str(), 0644);
			if (success != 0)
				std::cerr << "couln't manipulate proxy permissions!" << std::endl;
		
			std::cout << "adding link for " << iter->proxyfiedScriptName << std::endl;
			if (hPsScriptDir)
				bool link_successful_created = createScriptForwarder(iter->proxyfiedScriptName);
		}
	}
	
	//link all remaining (unused) scripts from proxifiedScripts
	if (hPsScriptDir){
		while (entry = readdir(hPsScriptDir)){
			stat((this->cfg_dir+"/proxifiedScripts/"+entry->d_name).c_str(), &fileProperties);
			if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
				bool success = createScriptForwarder(entry->d_name);
				if (success)
					toplevelScriptCount++;
			}
		}
		closedir(hPsScriptDir);
	}

	this->send_new_load_progress(0.1);

	//execute scripts
	mkconfigProc = popen(this->mkconfig_cmd.c_str(), "r");
	readGeneratedFile(mkconfigProc, toplevelScriptCount);

	int success = pclose(mkconfigProc);
	if (success != 0 && !cancelThreadsRequested){
		if (connectedUI){
			this->message = mkconfig_cmd + gettext(" couldn't be executed successfully. You must run this as root!");
			connectedUI->event_thread_died();
		}
		return; //cancel this thread
	}

	this->send_new_load_progress(0.9);
	
	mkconfigProc = NULL;
	
	//restore configuration
	for (std::list<ToplevelScript>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->proxyfiedScriptName == ""){
			int success = chmod((this->cfg_dir+"/"+iter->name).c_str(), iter->permissions);
			if (success != 0)
				std::cerr << "couln't manipulate script permissions!" << std::endl;
		}
		else {
			int success = chmod((this->cfg_dir+"/"+iter->name).c_str(), iter->permissions);
			if (success != 0)
				std::cerr << "couln't manipulate proxy permissions!" << std::endl;
		
			unlink((this->cfg_dir+"/LS_"+iter->proxyfiedScriptName).c_str());
		}
	}

	//remove all links of remaining (unused) proxifiedScripts
	hPsScriptDir = opendir((this->cfg_dir+"/proxifiedScripts").c_str());
	if (hPsScriptDir){
		while (entry = readdir(hPsScriptDir)){
			stat((std::string(this->cfg_dir+"/proxifiedScripts/")+entry->d_name).c_str(), &fileProperties);
			if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
				unlink((std::string(this->cfg_dir+"/LS_")+entry->d_name).c_str()); //should not be successfull in every case
			}
		}
		closedir(hPsScriptDir);
	}

	this->send_new_load_progress(0.97);

	//execute proxies

	for (std::list<ToplevelScript>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->proxyfiedScriptName != ""){
			FILE* proxyFile = fopen((this->cfg_dir+"/"+iter->name).c_str(), "r");
			ProxyscriptData proxyData = parseProxyScript(proxyFile);
			fclose(proxyFile);
			if (proxyData.ruleString != ""){
				iter->entries = executeListRules(iter->entries, readRuleString(proxyData.ruleString.c_str()));
			}
		}
	}
	
	//compare to generated file
	FILE* oldConfigFile = fopen(output_config_file.c_str(), "r");
	if (oldConfigFile){
		GrubConfig oldConfig;
		oldConfig.readGeneratedFile(oldConfigFile);
		config_has_been_different_on_startup_but_unsaved = !this->compare(oldConfig);
		fclose(oldConfigFile);
	}
	this->send_new_load_progress(1);
}

void GrubConfig::readGeneratedFile(FILE* source, int toplevelScriptCount){
	GrubConfRow row;
	std::string scriptName;
	int i = 0;
	while (!cancelThreadsRequested && (row = readGrubConfRow(source))){
		if (row.text.substr(0,10) == ("### BEGIN ") && row.text.substr(row.text.length()-4,4) == " ###"){
			scriptName = row.text.substr(22, row.text.length()-26);
			if (scriptName[0] != '0'){ //ignore header scripts (00_header, 05_debian_theme)
				if (toplevelScriptCount != -1)
					this->send_new_load_progress(0.1 + (0.7 / toplevelScriptCount * ++i));
				this->realScripts[scriptName.substr(3)]; //make sure that empty scripts will be added to realScripts
			}
		}
		else if (row.text.substr(0, 10) == "menuentry ") {
			Entry grubEntry = readGrubEntry(source, row);
			if (scriptName.substr(0,3) == "LS_"){
				std::string proxyfiedScriptName = scriptName.substr(3);
				for (std::list<ToplevelScript>::iterator confIter = this->begin(); confIter != this->end(); confIter++){
					if (confIter->proxyfiedScriptName == proxyfiedScriptName){
						confIter->entries.push_back(grubEntry);
					}
				}
			}
			else {
				bool scriptFound = false;
				for (std::list<ToplevelScript>::iterator confIter = this->begin(); confIter != this->end(); confIter++){
					if (confIter->name == scriptName){
						confIter->entries.push_back(grubEntry);
						scriptFound = true;
						break;
					}
				}
				if (!scriptFound){ //if the config isn't read before, script will not be found
					this->push_back(ToplevelScript(scriptName, "", 0755));
					this->back().entries.push_back(grubEntry);
				}
			}
			this->realScripts[scriptName.substr(3)].push_back(grubEntry);
			
			if (toplevelScriptCount != -1)
				this->send_new_load_progress(0.1 + (0.7 / toplevelScriptCount * i));
		}
	}
}

bool GrubConfig::compare(GrubConfig const& other) const {
	GrubConfig::const_iterator self_tls_iter = this->begin();
	GrubConfig::const_iterator other_tls_iter = other.begin();
	
	bool differenceFound = false;
	while (!differenceFound && self_tls_iter != this->end() && other_tls_iter != other.end()){
		if (self_tls_iter->name == other_tls_iter->name){
			EntryList::const_iterator self_entry_iter = self_tls_iter->entries.begin();
			EntryList::const_iterator other_entry_iter = other_tls_iter->entries.begin();
			while (!differenceFound && self_entry_iter != self_tls_iter->entries.end() && other_entry_iter != other_tls_iter->entries.end()){
				if (!self_entry_iter->disabled && !other_entry_iter->disabled){
					if (self_entry_iter->outputName != other_entry_iter->outputName || self_entry_iter->extension != other_entry_iter->extension || self_entry_iter->content != other_entry_iter->content)
						differenceFound = true;
					
					self_entry_iter++;
					other_entry_iter++;
				}
				else { //ignore disabled entries
					if (self_entry_iter->disabled)
						self_entry_iter++;
					if (other_entry_iter->disabled)
						other_entry_iter++;
				}
			}
		}
		else {
			differenceFound = true;
		}
		
		self_tls_iter++;
		other_tls_iter++;
	}
	return !differenceFound;
}

bool GrubConfig::createScriptForwarder(std::string scriptName){
	std::string outputFilePath = this->cfg_dir+"/LS_"+scriptName;
	FILE* existingScript = fopen(outputFilePath.c_str(), "r");
	if (existingScript == NULL){
		FILE* fwdScript = fopen(outputFilePath.c_str(), "w");
		fputs("#!/bin/sh\n", fwdScript);
		fputs((cfg_dir_noprefix+"/proxifiedScripts/"+scriptName).c_str(), fwdScript);
		fclose(fwdScript);
		chmod(outputFilePath.c_str(), 0755);
		return true;
	}
	else {
		fclose(existingScript);
		return false;
	}
}

void GrubConfig::threadable_install(std::string device){
	this->install_result = install(device);
	if (connectedUI)
		connectedUI->event_grub_install_ready();
}

std::string GrubConfig::install(std::string device){
	FILE* install_proc = popen((this->install_cmd+" '"+device+"' 2>&1").c_str(), "r");
	std::string output;
	int c;
	while ((c = fgetc(install_proc)) != EOF){
		output += c;
	}
	int success = pclose(install_proc);
	if (success == 0)
		return ""; //empty return string = no error
	else
		return output;
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
	fputs(this->cfg_dir_noprefix.c_str(), proxyFile);
	fputs("/proxifiedScripts/", proxyFile);
	fputs(script->proxyfiedScriptName.c_str(), proxyFile);
	fputs((" | "+this->cfg_dir_noprefix+"/bin/grubcfg_proxy \"").c_str(), proxyFile);
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
		DIR* hScriptDir = opendir(this->cfg_dir.c_str());
		while (entry = readdir(hScriptDir)){
			if (entry->d_name[2] == '_' && entry->d_name[0] != '0'){ //only script, without ignore header scripts
				stat((this->cfg_dir+"/"+entry->d_name).c_str(), &fileProperties);
				if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
					unmodifiedScripts.push_back(entry->d_name);
				}
			}
		}
		closedir(hScriptDir);
	}
	
	int mkdir_result = mkdir((this->cfg_dir+"/proxifiedScripts").c_str(), 0755); //create this directory if it doesn't allready exist
	
	int proxyCount = 0;
	for (std::list<ToplevelScript>::iterator iter = this->begin(); iter != this->end(); iter++){
		unmodifiedScripts.remove(iter->name);
		std::ostringstream newName;
		newName << iter->index << "_" << iter->getBasename();
		if (iter->isProxy)
			newName << "_proxy";
		
		bool file_exists = false;
		FILE* f = fopen((this->cfg_dir+"/"+iter->name).c_str(), "r");
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
				chmod((this->cfg_dir+"/"+iter->name).c_str(), 0755);
				res = rename((this->cfg_dir+"/"+iter->name).c_str(), (this->cfg_dir+"/proxifiedScripts/"+iter->proxyfiedScriptName).c_str());
			}
			if (res == 0){
				iter->name = newName.str();
				FILE* proxyFile = fopen((this->cfg_dir+"/"+iter->name).c_str(), "w");
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
					rename((this->cfg_dir+"/"+iter->name).c_str(), (this->cfg_dir+"/"+newName.str()).c_str());
					std::cout << "renaming " << iter->name << " to " << newName.str() << std::endl;
				}
				else {
					rename((this->cfg_dir+"/proxifiedScripts/"+iter->getBasename()).c_str(), (this->cfg_dir+"/"+newName.str()).c_str());
				}
				iter->name = newName.str();
			}
			if (iter->isProxy && iter->proxyfiedScriptName != ""){
				FILE* proxyFile = fopen((this->cfg_dir+"/"+iter->name).c_str(), "w");
				generateProxy(proxyFile, &*iter);
				proxyCount++;
				fclose(proxyFile);
			}
		}
		std::cout << "setting permisssions for " << iter->name << ": " << iter->permissions << std::endl;
		chmod((this->cfg_dir+"/"+iter->name).c_str(), iter->permissions);
	}
	send_new_save_progress(0.2);
	
	for (std::list<std::string>::iterator iter = unmodifiedScripts.begin(); iter != unmodifiedScripts.end(); iter++){
		if (getProxifiedScriptName(this->cfg_dir+"/"+(*iter)) == ""){
			chmod((this->cfg_dir+"/"+(*iter)).c_str(), 0755);
			rename((this->cfg_dir+"/"+(*iter)).c_str(), (this->cfg_dir+"/proxifiedScripts/"+iter->substr(3)).c_str());
		}
		else
			unlink((this->cfg_dir+"/"+(*iter)).c_str());
	}
	
	//remove "proxifiedScripts" dir, if empty
	
	{
		int proxifiedScriptCount = 0;
		struct dirent *entry;
		struct stat fileProperties;
		DIR* hScriptDir = opendir((this->cfg_dir+"/proxifiedScripts").c_str());
		while (entry = readdir(hScriptDir)){
			if (std::string(entry->d_name) != "." && std::string(entry->d_name) != ".."){
				proxifiedScriptCount++;
			}
		}
		closedir(hScriptDir);
		
		if (proxifiedScriptCount == 0)
			rmdir((this->cfg_dir+"/proxifiedScripts").c_str());
	}
	
	//add or remove proxy binary
	
	FILE* proxyBin = fopen((this->cfg_dir+"/bin/grubcfg_proxy").c_str(), "r");
	bool proxybin_exists = proxyBin != NULL;
	bool proxy_is_dummy = false;
	std::string dummyproxy_code = "#!/bin/sh\ncat\n";
	std::string proxy_code;
	
	if (proxyBin){
		std::cerr << "proxybin does already exist!" << std::endl;
		int c;
		for (int i = 0; i < dummyproxy_code.length() && (c = fgetc(proxyBin)) != EOF; i++)
			proxy_code += c;
		
		if (proxy_code == dummyproxy_code)
			proxy_is_dummy = true;
		else
			std::cerr << proxy_code << " : " << dummyproxy_code << std::endl;
		fclose(proxyBin);
	}
	
	if (proxyCount != 0 && (!proxybin_exists || proxy_is_dummy)){
		std::cout << "proxyCount: " << proxyCount << std::endl;
		//copy proxy
		int bin_mk_success = mkdir((this->cfg_dir+"/bin").c_str(), 0755);

		FILE* proxyBinSource = fopen((std::string(LIBDIR)+"/grubcfg-proxy").c_str(), "r");
		
		if (proxyBinSource){
			FILE* proxyBinTarget = fopen((this->cfg_dir+"/bin/grubcfg_proxy").c_str(), "w");
			if (proxyBinTarget){
				int c;
				while ((c = fgetc(proxyBinSource)) != EOF){
					fputc(c, proxyBinTarget);
				}
				fclose(proxyBinTarget);
				chmod((this->cfg_dir+"/bin/grubcfg_proxy").c_str(), 0755);
			}
			else
				std::cerr << "could not open proxy output file!" << std::endl;
			fclose(proxyBinSource);
		}
		else {
			std::cerr << "proxy could not be copied, generating dummy!" << std::endl;
			FILE* proxyBinTarget = fopen((this->cfg_dir+"/bin/grubcfg_proxy").c_str(), "w");
			if (proxyBinTarget){
				fputs(dummyproxy_code.c_str(), proxyBinTarget);
				error_proxy_not_found = true;
				fclose(proxyBinTarget);
				chmod((this->cfg_dir+"/bin/grubcfg_proxy").c_str(), 0755);
			}
			else
				std::cerr << "coundn't create proxy!" << std::endl;
		}
	}
	else if (proxyCount == 0 && proxybin_exists){
		//the following commands are only cleanup… no problem, when they fail
		unlink((this->cfg_dir+"/bin/grubcfg_proxy").c_str());
		rmdir((this->cfg_dir+"/bin").c_str());
	}
	
	//run update-grub
	FILE* saveProc = popen((update_cmd+" 2>&1").c_str(), "r");
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
	config_has_been_different_on_startup_but_unsaved = false;
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
