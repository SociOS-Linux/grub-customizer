/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ListCfg.h"

Model_ListCfg::Model_ListCfg(Model_Env& env)
 : error_proxy_not_found(false),
 progress(0),
 cancelThreadsRequested(false), verbose(true), env(env), eventListener(NULL),
 mutex(NULL), errorLogFile(ERROR_LOG_FILE), ignoreLock(false), progress_pos(0), progress_max(0)
{}

void Model_ListCfg::setEventListener(MainController& eventListener) {
	this->eventListener = &eventListener;
}

void Model_ListCfg::setMutex(Mutex& mutex) {
	this->mutex = &mutex;
}

void Model_ListCfg::setLogger(Logger& logger) {
	this->CommonClass::setLogger(logger);
	this->proxies.setLogger(logger);
	this->repository.setLogger(logger);
}

void Model_ListCfg::lock(){
	if (this->ignoreLock)
		return;
	if (this->mutex == NULL)
		throw ConfigException("missing mutex", __FILE__, __LINE__);
	this->mutex->lock();
}
bool Model_ListCfg::lock_if_free(){
	if (this->ignoreLock)
		return true;
	if (this->mutex == NULL)
		throw ConfigException("missing mutex", __FILE__, __LINE__);
	return this->mutex->trylock();
}
void Model_ListCfg::unlock(){
	if (this->ignoreLock)
		return;
	if (this->mutex == NULL)
		throw ConfigException("missing mutex", __FILE__, __LINE__);
	this->mutex->unlock();
}

bool Model_ListCfg::createScriptForwarder(std::string const& scriptName) const {
	//replace: $cfg_dir/proxifiedScripts/ -> $cfg_dir/LS_
	std::string scriptNameNoPath = scriptName.substr((this->env.cfg_dir+"/proxifiedScripts/").length());
	std::string outputFilePath = this->env.cfg_dir+"/LS_"+scriptNameNoPath;
	FILE* existingScript = fopen(outputFilePath.c_str(), "r");
	if (existingScript == NULL){
		FILE* fwdScript = fopen(outputFilePath.c_str(), "w");
		if (fwdScript){
			fputs("#!/bin/sh\n", fwdScript);
			fputs(("'"+scriptName.substr(env.cfg_dir_prefix.length())+"'").c_str(), fwdScript);
			fclose(fwdScript);
			chmod(outputFilePath.c_str(), 0755);
			return true;
		}
		else
			return false;
	}
	else {
		fclose(existingScript);
		return false;
	}
}

bool Model_ListCfg::removeScriptForwarder(std::string const& scriptName) const {
	std::string scriptNameNoPath = scriptName.substr((this->env.cfg_dir+"/proxifiedScripts/").length());
	std::string filePath = this->env.cfg_dir+"/LS_"+scriptNameNoPath;
	return unlink(filePath.c_str()) == 0;
}

std::string Model_ListCfg::readScriptForwarder(std::string const& scriptForwarderFilePath) const {
	std::string result;
	FILE* scriptForwarderFile = fopen(scriptForwarderFilePath.c_str(), "r");
	if (scriptForwarderFile){
		int c;
		while ((c = fgetc(scriptForwarderFile)) != EOF && c != '\n'){} //skip first line
		if (c != EOF)
			while ((c = fgetc(scriptForwarderFile)) != EOF && c != '\n'){result += char(c);} //read second line (=path)
		fclose(scriptForwarderFile);
	}
	if (result.length() >= 3) {
		return result.substr(1, result.length()-2);
	} else {
		return "";
	}
}

void Model_ListCfg::load(bool preserveConfig){
	if (!preserveConfig){
		send_new_load_progress(0);

		DIR* hGrubCfgDir = opendir(this->env.cfg_dir.c_str());

		if (!hGrubCfgDir){
			throw DirectoryNotFoundException("grub cfg dir not found", __FILE__, __LINE__);
		}

		//load scripts
		this->log("loading scripts…", Logger::EVENT);
		this->lock();
		repository.load(this->env.cfg_dir, false);
		repository.load(this->env.cfg_dir+"/proxifiedScripts", true);
		this->unlock();
		send_new_load_progress(0.05);
	
	
		//load proxies
		this->log("loading proxies…", Logger::EVENT);
		this->lock();
		struct dirent *entry;
		struct stat fileProperties;
		while ((entry = readdir(hGrubCfgDir))){
			stat((this->env.cfg_dir+"/"+entry->d_name).c_str(), &fileProperties);
			if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
				if (entry->d_name[2] == '_'){ //check whether it's an script (they should be named XX_scriptname)…
					this->proxies.push_back(Model_Proxy());
					this->proxies.back().fileName = this->env.cfg_dir+"/"+entry->d_name;
					this->proxies.back().index = (entry->d_name[0]-'0')*10 + (entry->d_name[1]-'0');
					this->proxies.back().permissions = fileProperties.st_mode & ~S_IFMT;
				
					FILE* proxyFile = fopen((this->env.cfg_dir+"/"+entry->d_name).c_str(), "r");
					Model_ProxyScriptData data(proxyFile);
					fclose(proxyFile);
					if (data){
						this->proxies.back().dataSource = repository.getScriptByFilename(this->env.cfg_dir_prefix+data.scriptCmd);
						this->proxies.back().importRuleString(data.ruleString.c_str());
					}
					else {
						this->proxies.back().dataSource = repository.getScriptByFilename(this->env.cfg_dir+"/"+entry->d_name);
						this->proxies.back().importRuleString("+*"); //it's no proxy, so accept all
					}
				
				}
			}
		}
		closedir(hGrubCfgDir);
		this->proxies.sort();
		this->unlock();
	}
	else {
		this->lock();
		proxies.unsync_all();
		repository.deleteAllEntries();
		this->unlock();
	}

	//create proxifiedScript links & chmod other files
	this->log("creating proxifiedScript links & chmodding other files…", Logger::EVENT);

	this->lock();
	for (Model_Repository::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++){
		if (iter->isInScriptDir(env.cfg_dir)){
			//createScriptForwarder & disable proxies
			createScriptForwarder(iter->fileName);
			std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*iter);
			for (std::list<Model_Proxy*>::iterator piter = relatedProxies.begin(); piter != relatedProxies.end(); piter++){
				int res = chmod((*piter)->fileName.c_str(), 0644);
			}
		} else {
			//enable scripts (unproxified), in this case, Proxy::fileName == Script::fileName
			chmod(iter->fileName.c_str(), 0755);
		}
	}
	this->unlock();
	send_new_load_progress(0.1);

	//run mkconfig
	this->log("running " + this->env.mkconfig_cmd, Logger::EVENT);
	FILE* mkconfigProc = popen((this->env.mkconfig_cmd + " 2> " + this->errorLogFile).c_str(), "r");
	readGeneratedFile(mkconfigProc);
	
	int success = pclose(mkconfigProc);
	if (success != 0 && !cancelThreadsRequested){
		throw CmdExecException("failed running " + this->env.mkconfig_cmd, __FILE__, __LINE__);
	} else {
		remove(errorLogFile.c_str()); //remove file, if everything was ok
	}
	this->log("mkconfig successfull completed", Logger::INFO);

	this->send_new_load_progress(0.9);

	mkconfigProc = NULL;
	
	this->env.useDirectBackgroundProps = this->repository.getScriptByName("debian_theme") == NULL;
	if (this->env.useDirectBackgroundProps) {
		this->log("using simple background image settings", Logger::INFO);
	} else {
		this->log("using /usr/share/desktop-base/grub_background.sh to configure colors and the background image", Logger::INFO);
	}

	
	//restore old configuration
	this->log("restoring grub configuration", Logger::EVENT);
	this->lock();
	for (Model_Repository::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++){
		if (iter->isInScriptDir(env.cfg_dir)){
			//removeScriptForwarder & reset proxy permissions
			bool result = removeScriptForwarder(iter->fileName);
			if (!result) {
				this->log("removing of script forwarder not successful!", Logger::ERROR);
			}
		}
		std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*iter);
		for (std::list<Model_Proxy*>::iterator piter = relatedProxies.begin(); piter != relatedProxies.end(); piter++){
			chmod((*piter)->fileName.c_str(), (*piter)->permissions);
		}
	}
	this->unlock();
	
	this->log("loading completed", Logger::EVENT);
	send_new_load_progress(1);
}


void Model_ListCfg::readGeneratedFile(FILE* source, bool createScriptIfNotFound, bool createProxyIfNotFound){
	Model_Entry_Row row;
	Model_Script* script;
	int i = 0;
	bool inScript = false;
	std::string plaintextBuffer = "";
	int innerCount = 0;
	double progressbarScriptSpace = 0.7 / this->repository.size();
	while (!cancelThreadsRequested && (row = Model_Entry_Row(source))){
		if (!inScript && row.text.substr(0,10) == ("### BEGIN ") && row.text.substr(row.text.length()-4,4) == " ###"){
			this->lock();
			if (script) {
				if (plaintextBuffer != "") {
					Model_Entry newEntry("#text", "", plaintextBuffer, Model_Entry::PLAINTEXT);
					if (this->hasLogger()) {
						newEntry.setLogger(this->getLogger());
					}
					script->entries().push_front(newEntry);
				}
				this->proxies.sync_all(true, true, script);
			}
			plaintextBuffer = "";
			std::string scriptName = row.text.substr(10, row.text.length()-14);
			std::string prefix = this->env.cfg_dir_prefix;
			std::string realScriptName = prefix+scriptName;
			if (realScriptName.substr(0, (this->env.cfg_dir+"/LS_").length()) == this->env.cfg_dir+"/LS_"){
				realScriptName = prefix+readScriptForwarder(realScriptName);
			}
			script = repository.getScriptByFilename(realScriptName, createScriptIfNotFound);
			if (createScriptIfNotFound && createProxyIfNotFound){ //for the compare-configuration
				this->proxies.push_back(Model_Proxy(*script));
			}
			this->unlock();
			if (script){
				this->send_new_load_progress(0.1 + (progressbarScriptSpace * ++i + (progressbarScriptSpace/10*innerCount)), script->name, i, this->repository.size());
			}
			inScript = true;
		} else if (inScript && row.text.substr(0,8) == ("### END ") && row.text.substr(row.text.length()-4,4) == " ###") {
			inScript = false;
			innerCount = 0;
		} else if (script != NULL && row.text.substr(0, 10) == "menuentry ") {
			this->lock();
			if (innerCount < 10) {
				innerCount++;
			}
			Model_Entry newEntry(source, row, this->getLoggerPtr());
			script->entries().push_back(newEntry);
			this->proxies.sync_all(false, false, script);
			this->unlock();
			this->send_new_load_progress(0.1 + (progressbarScriptSpace * i + (progressbarScriptSpace/10*innerCount)), script->name, i, this->repository.size());
		} else if (script != NULL && row.text.substr(0, 8) == "submenu ") {
			this->lock();
			Model_Entry newEntry(source, row, this->getLoggerPtr());
			script->entries().push_back(newEntry);
			this->proxies.sync_all(false, false, script);
			this->unlock();
			this->send_new_load_progress(0.1 + (progressbarScriptSpace * i + (progressbarScriptSpace/10*innerCount)), script->name, i, this->repository.size());
		} else if (inScript) { //Plaintext
			plaintextBuffer += row.text + "\n";
		}
	}
	this->lock();
	if (script) {
		if (plaintextBuffer != "") {
			Model_Entry newEntry("#text", "", plaintextBuffer, Model_Entry::PLAINTEXT);
			if (this->hasLogger()) {
				newEntry.setLogger(this->getLogger());
			}
			script->entries().push_front(newEntry);
		}
		this->proxies.sync_all(true, true, script);
	}

	// sync all (including foreign entries)
	this->proxies.sync_all(true, true, NULL, this->repository.getScriptPathMap());

	this->unlock();
}

void Model_ListCfg::save(){
	send_new_save_progress(0);
	std::map<std::string, int> samename_counter;
	proxies.deleteAllProxyscriptFiles();  //delete all proxies to get a clean file system
	proxies.clearTrash(); //delete all files of removed proxies
	
	// create virtual custom scripts on file system
	for (std::list<Model_Script>::iterator scriptIter = this->repository.begin(); scriptIter != this->repository.end(); scriptIter++) {
		if (scriptIter->isCustomScript && scriptIter->fileName == "") {
			scriptIter->fileName = this->env.cfg_dir + "/IN_" + scriptIter->name;
			this->repository.createScript(*scriptIter, "");
		}
	}

	for (Model_Repository::iterator script_iter = repository.begin(); script_iter != repository.end(); script_iter++)
		script_iter->moveToBasedir(this->env.cfg_dir);

	send_new_save_progress(0.1);

	int mkdir_result = mkdir((this->env.cfg_dir+"/proxifiedScripts").c_str(), 0755); //create this directory if it doesn't already exist

	// get new script locations
	std::map<Model_Script const*, std::string> scriptTargetMap; // scripts and their target directories
	for (Model_Repository::iterator script_iter = repository.begin(); script_iter != repository.end(); script_iter++) {
		std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*script_iter);
		if (proxies.proxyRequired(*script_iter)){
			scriptTargetMap[&*script_iter] = this->env.cfg_dir+"/proxifiedScripts/"+Model_PscriptnameTranslator::encode(script_iter->name, samename_counter[script_iter->name]++);
		} else {
			std::ostringstream nameStream;
			nameStream << std::setw(2) << std::setfill('0') << relatedProxies.front()->index << "_" << script_iter->name;
			std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*script_iter);
			scriptTargetMap[&*script_iter] = this->env.cfg_dir+"/"+nameStream.str();
		}
	}

	// move scripts and create proxies
	int proxyCount = 0;
	for (Model_Repository::iterator script_iter = repository.begin(); script_iter != repository.end(); script_iter++){
		std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*script_iter);
		if (proxies.proxyRequired(*script_iter)){
			script_iter->moveFile(scriptTargetMap[&*script_iter], 0755);
			for (std::list<Model_Proxy*>::iterator proxy_iter = relatedProxies.begin(); proxy_iter != relatedProxies.end(); proxy_iter++){
				std::map<Model_Entry const*, Model_Script const*> entrySourceMap = this->getEntrySources(**proxy_iter);
				std::ostringstream nameStream;
				nameStream << std::setw(2) << std::setfill('0') << (*proxy_iter)->index << "_" << script_iter->name << "_proxy";
				(*proxy_iter)->generateFile(this->env.cfg_dir+"/"+nameStream.str(), this->env.cfg_dir_prefix.length(), this->env.cfg_dir_noprefix, entrySourceMap, scriptTargetMap);
				proxyCount++;
			}
		}
		else {
			if (relatedProxies.size() == 1){
				script_iter->moveFile(scriptTargetMap[&*script_iter], relatedProxies.front()->permissions);
			}
			else {
				this->log("GrublistCfg::save: cannot move proxy… only one expected!", Logger::ERROR);
			}
		}	
	}
	send_new_save_progress(0.2);


	//remove "proxifiedScripts" dir, if empty
	
	{
		int proxifiedScriptCount = 0;
		struct dirent *entry;
		struct stat fileProperties;
		DIR* hScriptDir = opendir((this->env.cfg_dir+"/proxifiedScripts").c_str());
		while ((entry = readdir(hScriptDir))) {
			if (std::string(entry->d_name) != "." && std::string(entry->d_name) != ".."){
				proxifiedScriptCount++;
			}
		}
		closedir(hScriptDir);
		
		if (proxifiedScriptCount == 0)
			rmdir((this->env.cfg_dir+"/proxifiedScripts").c_str());
	}

	//add or remove proxy binary
	
	FILE* proxyBin = fopen((this->env.cfg_dir+"/bin/grubcfg_proxy").c_str(), "r");
	bool proxybin_exists = proxyBin != NULL;
	std::string dummyproxy_code = "#!/bin/sh\ncat\n";
	
	/**
	 * copy the grub customizer proxy, if required
	 */
	if (proxyCount != 0){
		// create the bin subdirectory - may already exist
		int bin_mk_success = mkdir((this->env.cfg_dir+"/bin").c_str(), 0755);

		FILE* proxyBinSource = fopen((std::string(LIBDIR)+"/grubcfg-proxy").c_str(), "r");
		
		if (proxyBinSource){
			FILE* proxyBinTarget = fopen((this->env.cfg_dir+"/bin/grubcfg_proxy").c_str(), "w");
			if (proxyBinTarget){
				int c;
				while ((c = fgetc(proxyBinSource)) != EOF){
					fputc(c, proxyBinTarget);
				}
				fclose(proxyBinTarget);
				chmod((this->env.cfg_dir+"/bin/grubcfg_proxy").c_str(), 0755);
			} else {
				this->log("could not open proxy output file!", Logger::ERROR);
			}
			fclose(proxyBinSource);
		} else {
			this->log("proxy could not be copied, generating dummy!", Logger::ERROR);
			FILE* proxyBinTarget = fopen((this->env.cfg_dir+"/bin/grubcfg_proxy").c_str(), "w");
			if (proxyBinTarget){
				fputs(dummyproxy_code.c_str(), proxyBinTarget);
				error_proxy_not_found = true;
				fclose(proxyBinTarget);
				chmod((this->env.cfg_dir+"/bin/grubcfg_proxy").c_str(), 0755);
			} else {
				this->log("coundn't create proxy!", Logger::ERROR);
			}
		}
	}
	else if (proxyCount == 0 && proxybin_exists){
		//the following commands are only cleanup… no problem, when they fail
		unlink((this->env.cfg_dir+"/bin/grubcfg_proxy").c_str());
		rmdir((this->env.cfg_dir+"/bin").c_str());
	}

	//update modified "custom" scripts
	for (std::list<Model_Script>::iterator scriptIter = this->repository.begin(); scriptIter != this->repository.end(); scriptIter++) {
		if (scriptIter->isCustomScript && scriptIter->isModified()) {
			this->log("modifying script \"" + scriptIter->name + "\"", Logger::INFO);
			assert(scriptIter->fileName != "");
			Model_Proxy dummyProxy(*scriptIter);
			std::ofstream scriptStream(scriptIter->fileName.c_str());
			scriptStream << CUSTOM_SCRIPT_SHEBANG << "\n" << CUSTOM_SCRIPT_PREFIX << "\n";
			for (std::list<Model_Rule>::iterator ruleIter = dummyProxy.rules.begin(); ruleIter != dummyProxy.rules.end(); ruleIter++) {
				ruleIter->print(scriptStream);
				if (ruleIter->dataSource) {
					ruleIter->dataSource->isModified = false;
				}
			}
		}
	}


	//run update-grub
	FILE* saveProc = popen((env.update_cmd+" 2>&1").c_str(), "r");
	if (saveProc) {
		int c;
		std::string row = "";
		while ((c = fgetc(saveProc)) != EOF) {
			if (c == '\n') {
				send_new_save_progress(0.5); //a gui should use pulse() instead of set_fraction
				this->log(row, Logger::INFO);
				row = "";
			} else {
				row += char(c);
			}
		}
		pclose(saveProc);
	}
	send_new_save_progress(1);
}

std::map<Model_Entry const*, Model_Script const*> Model_ListCfg::getEntrySources(Model_Proxy const& proxy, Model_Rule const* parent) const {
	std::list<Model_Rule> const& list = parent ? parent->subRules : proxy.rules;
	std::map<Model_Entry const*, Model_Script const*> result;
	assert(proxy.dataSource != NULL);
	for (std::list<Model_Rule>::const_iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->dataSource && !proxy.ruleIsFromOwnScript(*iter)) {
			Model_Script const* script = this->repository.getScriptByEntry(*iter->dataSource);
			if (script != NULL) {
				result[iter->dataSource] = script;
			} else {
				this->log("error finding the associated script! (" + iter->outputName + ")", Logger::WARNING);
			}
		} else if (iter->type == Model_Rule::SUBMENU) {
			std::map<Model_Entry const*, Model_Script const*> subResult = this->getEntrySources(proxy, &*iter);
			if (subResult.size()) {
				result.insert(subResult.begin(), subResult.end());
			}
		}
	}
	return result;
}

bool Model_ListCfg::loadStaticCfg(){
	FILE* oldConfigFile = fopen(env.output_config_file.c_str(), "r");
	if (oldConfigFile){
		this->readGeneratedFile(oldConfigFile, true, true);
		fclose(oldConfigFile);
		return true;
	}
	return false;
}

void Model_ListCfg::renameRule(Model_Rule* rule, std::string const& newName){
	rule->outputName = newName;
}

std::string Model_ListCfg::getGrubErrorMessage() const {
	FILE* errorLogFile = fopen(this->errorLogFile.c_str(), "r");
	std::string errorMessage;
	int c;
	while ((c = fgetc(errorLogFile)) != EOF) {
		errorMessage += char(c);
	}
	fclose(errorLogFile);
	return errorMessage;
}

bool Model_ListCfg::compare(Model_ListCfg const& other) const {
	std::list<const Model_Rule*> rlist[2];
	for (int i = 0; i < 2; i++){
		const Model_ListCfg* gc = i == 0 ? this : &other;
		for (Model_Proxylist::const_iterator piter = gc->proxies.begin(); piter != gc->proxies.end(); piter++){
			assert(piter->dataSource != NULL);
			if (piter->isExecutable() && piter->dataSource){
				if (piter->dataSource->fileName == "") { // if the associated file isn't found
					return false;
				}
				std::string fname = piter->dataSource->fileName.substr(other.env.cfg_dir.length()+1);
				if (i == 0 || (fname[0] >= '1' && fname[0] <= '9' && fname[1] >= '0' && fname[1] <= '9' && fname[2] == '_')) {
					std::list<Model_Rule const*> comparableRules = this->getComparableRules(piter->rules);
					rlist[i].splice(rlist[i].end(), comparableRules);
				}
			}
		}
	}
	return Model_ListCfg::compareLists(rlist[0], rlist[1]);
}

std::list<Model_Rule const*> Model_ListCfg::getComparableRules(std::list<Model_Rule> const& list) {
	std::list<Model_Rule const*> result;
	for (std::list<Model_Rule>::const_iterator riter = list.begin(); riter != list.end(); riter++){
		if (((riter->type == Model_Rule::NORMAL && riter->dataSource) || riter->type == Model_Rule::SUBMENU) && riter->isVisible){
			result.push_back(&*riter);
		}
	}
	return result;
}

bool Model_ListCfg::compareLists(std::list<Model_Rule const*> a, std::list<Model_Rule const*> b) {
	if (a.size() != b.size())
		return false;

	std::list<const Model_Rule*>::iterator self_iter = a.begin(), other_iter = b.begin();
	while (self_iter != a.end() && other_iter != b.end()){
		if ((*self_iter)->type != (*other_iter)->type) {
			return false;
		}
		assert((*self_iter)->type == (*other_iter)->type);
		//check this Rule
		if ((*self_iter)->outputName != (*other_iter)->outputName)
			return false;
		if ((*self_iter)->dataSource) {
			if ((*self_iter)->dataSource->extension != (*other_iter)->dataSource->extension)
				return false;
			if ((*self_iter)->dataSource->content != (*other_iter)->dataSource->content)
				return false;
			if ((*self_iter)->dataSource->type != (*other_iter)->dataSource->type)
				return false;
		}
		//check rules inside the submenu
		if ((*self_iter)->type == Model_Rule::SUBMENU && !Model_ListCfg::compareLists(Model_ListCfg::getComparableRules((*self_iter)->subRules), Model_ListCfg::getComparableRules((*other_iter)->subRules))) {
			return false;
		}
		self_iter++;
		other_iter++;
	}
	return true;
}


void Model_ListCfg::send_new_load_progress(double newProgress, std::string scriptName, int current, int max){
	if (this->eventListener != NULL){
		this->progress = newProgress;
		this->progress_name = scriptName;
		this->progress_pos = current;
		this->progress_max = max;
		this->eventListener->syncLoadStateThreadedAction();
	}
	else if (this->verbose) {
		this->log("cannot show updated load progress - no UI connected!", Logger::ERROR);
	}
}

void Model_ListCfg::send_new_save_progress(double newProgress){
	if (this->eventListener != NULL){
		this->progress = newProgress;
		this->eventListener->syncSaveStateThreadedAction();
	}
	else if (this->verbose) {
		this->log("cannot show updated save progress - no UI connected!", Logger::ERROR);
	}
}

void Model_ListCfg::cancelThreads(){
	cancelThreadsRequested = true;
}


void Model_ListCfg::reset(){
	this->lock();
	this->repository.clear();
	this->proxies.clear();
	this->unlock();
}

double Model_ListCfg::getProgress() const {
	return progress;
}

std::string Model_ListCfg::getProgress_name() const {
	return progress_name;
}
int Model_ListCfg::getProgress_pos() const {
	return progress_pos;
}
int Model_ListCfg::getProgress_max() const {
	return progress_max;
}

void Model_ListCfg::renumerate(){
	short int i = 0;
	for (Model_Proxylist::iterator iter = this->proxies.begin(); iter != this->proxies.end(); iter++){
		if (i <= 0 && iter->dataSource && iter->dataSource->name == "header") {
			i = 0;
		} else if (i <= 5 && iter->dataSource && iter->dataSource->name == "debian_theme") {
			i = 5;
		} else if (i <= 10) {
			i = 10;
		}
		iter->index = i++;
	}
	this->proxies.sort();
}

Model_Rule& Model_ListCfg::moveRule(Model_Rule* rule, int direction){
	try {
		return this->proxies.getProxyByRule(rule)->moveRule(rule, direction);
	} catch (NoMoveTargetException const& e) {
		Model_Proxy* proxy = this->proxies.getProxyByRule(rule);
		Model_Rule* parent = NULL;
		try {
			parent = proxy->getParentRule(rule);
		} catch (ItemNotFoundException const& e) {/* do nothing */}

		try {
			std::list<Model_Rule>::iterator nextRule = this->proxies.getNextVisibleRule(proxy->getListIterator(*rule, proxy->getRuleList(parent)), direction);
			if (nextRule->type != Model_Rule::SUBMENU) { // create new proxy
				std::list<Model_Rule>::iterator targetRule = proxy->rules.end();
				bool targetRuleFound = false;
				try {
					targetRule = this->proxies.getNextVisibleRule(nextRule, direction);
					targetRuleFound = true;
				} catch (NoMoveTargetException const& e) {
					// ignore GrublistCfg::NO_MOVE_TARGET_FOUND - occurs when previousRule is not found. But this isn't a problem
				}

				if (targetRuleFound && this->proxies.getProxyByRule(&*targetRule)->dataSource == this->proxies.getProxyByRule(&*rule)->dataSource) {
					Model_Proxy* targetProxy = this->proxies.getProxyByRule(&*targetRule);
					targetProxy->removeEquivalentRules(*rule);
					Model_Rule* newRule = NULL;
					if (direction == -1) {
						targetProxy->rules.push_back(*rule);
						newRule = &targetProxy->rules.back();
					} else {
						targetProxy->rules.push_front(*rule);
						newRule = &targetProxy->rules.front();
					}
					rule->isVisible = false;

					try {
						std::list<Model_Rule>::iterator previousRule = this->proxies.getNextVisibleRule(proxy->getListIterator(*rule, proxy->getRuleList(parent)), -direction);
						if (this->proxies.getProxyByRule(&*nextRule)->dataSource == this->proxies.getProxyByRule(&*previousRule)->dataSource) {
							this->proxies.getProxyByRule(&*previousRule)->removeEquivalentRules(*nextRule);
							if (direction == 1) {
								this->proxies.getProxyByRule(&*previousRule)->rules.push_back(*nextRule);
							} else {
								this->proxies.getProxyByRule(&*previousRule)->rules.push_front(*nextRule);
							}
							nextRule->isVisible = false;
							if (!this->proxies.getProxyByRule(&*nextRule)->hasVisibleRules()) {
								this->proxies.deleteProxy(this->proxies.getProxyByRule(&*nextRule));
							}
						}
					} catch (NoMoveTargetException const& e) {
						// ignore NoMoveTargetException - occurs when previousRule is not found. But this isn't a problem
					}

					// cleanup
					if (!proxy->hasVisibleRules()) {
						this->proxies.deleteProxy(proxy);
					}

					return *newRule;
				} else {
					std::list<Model_Rule>::iterator movedRule = this->proxies.moveRuleToNewProxy(*rule, direction);

					Model_Proxy* currentProxy = this->proxies.getProxyByRule(&*movedRule);

					std::list<Model_Rule>::iterator movedRule2 = this->proxies.moveRuleToNewProxy(*nextRule, -direction);
					this->renumerate();
					this->swapProxies(currentProxy, this->proxies.getProxyByRule(&*movedRule2));

					try {
						std::list<Model_Rule>::iterator prevPrevRule = this->proxies.getNextVisibleRule(movedRule2, -direction);

						if (this->proxies.getProxyByRule(&*prevPrevRule)->dataSource == this->proxies.getProxyByRule(&*movedRule2)->dataSource) {
							Model_Proxy* prevprev = this->proxies.getProxyByRule(&*prevPrevRule);
							prevprev->removeEquivalentRules(*movedRule2);
							if (direction == 1) {
								prevprev->rules.push_back(*movedRule2);
							} else {
								prevprev->rules.push_front(*movedRule2);
							}
							movedRule2->isVisible = false;
							if (!this->proxies.getProxyByRule(&*movedRule2)->hasVisibleRules()) {
								this->proxies.deleteProxy(this->proxies.getProxyByRule(&*movedRule2));
							}
						}
					} catch (NoMoveTargetException const& e) {
						// ignore NoMoveTargetException - occurs when prevPrevRule is not found. But this isn't a problem
					}

					return *movedRule;
				}
			} else { // convert existing proxy to multiproxy
				this->log("convert to multiproxy", Logger::INFO);
				std::list<Model_Proxy>::iterator proxyIter = this->proxies.getIter(proxy);

				Model_Rule* movedRule = NULL;
				Model_Proxy* target = NULL;
				if (direction == -1 && proxyIter != this->proxies.begin()) {
					proxyIter--;
					target = &*proxyIter;
					target->removeEquivalentRules(*rule);
					nextRule->subRules.push_back(*rule);
					if (rule->type == Model_Rule::SUBMENU) {
						proxy->removeForeignChildRules(*rule);
					}
					if ((rule->type == Model_Rule::SUBMENU && rule->subRules.size() != 0) || (rule->type != Model_Rule::SUBMENU && proxy->ruleIsFromOwnScript(*rule))) {
						rule->isVisible = false;
					} else {
						proxy->rules.pop_front();
					}
					movedRule = &nextRule->subRules.back();
					proxyIter++;
					proxyIter++; // go to the next proxy
				} else if (direction == 1 && proxyIter != this->proxies.end() && &*proxyIter != &this->proxies.back()) {
					proxyIter++;
					target = &*proxyIter;
					target->removeEquivalentRules(*rule);
					nextRule->subRules.push_front(*rule);
					if (rule->type == Model_Rule::SUBMENU) {
						proxy->removeForeignChildRules(*rule);
					}
					if ((rule->type == Model_Rule::SUBMENU && rule->subRules.size() != 0) || (rule->type != Model_Rule::SUBMENU && proxy->ruleIsFromOwnScript(*rule))) {
						rule->isVisible = false;
					} else {
						proxy->rules.pop_back();
					}
					movedRule = &nextRule->subRules.front();

					proxyIter--;
					proxyIter--; // go to the previous proxy
				} else {
					throw NoMoveTargetException("cannot move this rule", __FILE__, __LINE__);
				}

				if (!proxy->hasVisibleRules()) {
					if (proxyIter != this->proxies.end() && target->dataSource == proxyIter->dataSource) {
						target->merge(*proxyIter, direction);
						this->proxies.deleteProxy(&*proxyIter);
					}

					this->proxies.deleteProxy(proxy);
				}
				return *movedRule;
			}
		} catch (NoMoveTargetException const& e) {
			throw e;
		}
	} catch (MustBeProxyException const& e) {
		Model_Proxy* proxy = this->proxies.getProxyByRule(rule);
		Model_Rule* parent = NULL;
		try {
			parent = proxy->getParentRule(rule);
		} catch (ItemNotFoundException const& e) {/* do nothing */}

		Model_Rule* parentSubmenu = parent;
		try {
			std::list<Model_Rule>::iterator nextRule = this->proxies.getNextVisibleRule(proxy->getListIterator(*parent, proxy->rules), direction); // go forward

			if (this->proxies.getProxyByRule(&*nextRule) == this->proxies.getProxyByRule(&*rule)) {
				this->proxies.splitProxy(proxy, &*nextRule, direction);
			}
		} catch (NoMoveTargetException const& e) {
			// there's no next rule… no split required
		}

		std::list<Model_Proxy>::iterator nextProxy = this->proxies.getIter(this->proxies.getProxyByRule(&*rule));
		if (direction == 1) {
			nextProxy++;
		} else {
			nextProxy--;
		}

		Model_Rule* movedRule = NULL;
		if (nextProxy != this->proxies.end() && nextProxy->dataSource == this->repository.getScriptByEntry(*rule->dataSource)) {
			nextProxy->removeEquivalentRules(*rule);
			if (direction == 1) {
				nextProxy->rules.push_front(*rule);
				movedRule = &nextProxy->rules.front();
			} else {
				nextProxy->rules.push_back(*rule);
				movedRule = &nextProxy->rules.back();
			}
		} else {
			movedRule = &*this->proxies.moveRuleToNewProxy(*rule, direction, this->repository.getScriptByEntry(*rule->dataSource));
		}
		proxy->removeEquivalentRules(*rule);
		return *movedRule;
	}
	throw NoMoveTargetException("no move target found", __FILE__, __LINE__);
}

void Model_ListCfg::swapProxies(Model_Proxy* a, Model_Proxy* b){
	if (a->index == b->index) { // swapping has no effect if the indexes are identical
		this->renumerate();
	}
	int index1 = a->index;
	a->index = b->index;
	b->index = index1;
	this->proxies.sort();
}

Model_Rule* Model_ListCfg::createSubmenu(Model_Rule* position) {
	return this->proxies.getProxyByRule(position)->createSubmenu(position);
}

Model_Rule* Model_ListCfg::splitSubmenu(Model_Rule* child) {
	return this->proxies.getProxyByRule(child)->splitSubmenu(child);
}

bool Model_ListCfg::cfgDirIsClean(){
	DIR* hGrubCfgDir = opendir(this->env.cfg_dir.c_str());
	if (hGrubCfgDir){
		struct dirent *entry;
		struct stat fileProperties;
		while ((entry = readdir(hGrubCfgDir))){
			std::string fname = entry->d_name;
			if ((fname.length() >= 4 && fname.substr(0,3) == "LS_") || fname.substr(0,3) == "PS_" || fname.substr(0,3) == "DS_")
				return false;
		}
		closedir(hGrubCfgDir);
	}
	return true;
}
void Model_ListCfg::cleanupCfgDir(){
	this->log("cleaning up cfg dir!", Logger::IMPORTANT_EVENT);
	
	DIR* hGrubCfgDir = opendir(this->env.cfg_dir.c_str());
	if (hGrubCfgDir){
		struct dirent *entry;
		struct stat fileProperties;
		std::list<std::string> lsfiles, dsfiles, psfiles;
		std::list<std::string> proxyscripts;
		while ((entry = readdir(hGrubCfgDir))){
			std::string fname = entry->d_name;
			if (fname.length() >= 4){
				if (fname.substr(0,3) == "LS_")
					lsfiles.push_back(fname);
				else if (fname.substr(0,3) == "DS_")
					dsfiles.push_back(fname);
				else if (fname.substr(0,3) == "PS_")
					psfiles.push_back(fname);

				else if (fname[0] >= '1' && fname[0] <= '9' && fname[1] >= '0' && fname[1] <= '9' && fname[2] == '_')
					proxyscripts.push_back(fname);
			}
		}
		closedir(hGrubCfgDir);
		
		for (std::list<std::string>::iterator iter = lsfiles.begin(); iter != lsfiles.end(); iter++){
			this->log("deleting " + *iter, Logger::EVENT);
			unlink((this->env.cfg_dir+"/"+(*iter)).c_str());
		}
		//proxyscripts will be disabled before loading the config. While the provious mode will only be saved on the objects, every script should be made executable
		for (std::list<std::string>::iterator iter = proxyscripts.begin(); iter != proxyscripts.end(); iter++){
			this->log("re-activating " + *iter, Logger::EVENT);
			chmod((this->env.cfg_dir+"/"+(*iter)).c_str(), 0755);
		}

		//remove the DS_ prefix  (DS_10_foo -> 10_foo)
		for (std::list<std::string>::iterator iter = dsfiles.begin(); iter != dsfiles.end(); iter++) {
			this->log("renaming " + *iter, Logger::EVENT);
			rename((this->env.cfg_dir+"/"+(*iter)).c_str(), (this->env.cfg_dir+"/"+iter->substr(3)).c_str());
		}

		//remove the PS_ prefix and add index prefix (PS_foo -> 10_foo)
		int i = 20; //prefix
		for (std::list<std::string>::iterator iter = psfiles.begin(); iter != psfiles.end(); iter++) {
			this->log("renaming " + *iter, Logger::EVENT);
			std::string out = *iter;
			out.replace(0, 2, (std::string("") + char('0' + (i/10)%10) + char('0' + i%10)));
			rename((this->env.cfg_dir+"/"+(*iter)).c_str(), (this->env.cfg_dir+"/"+out).c_str());
			i++;
		}
	}
}

void Model_ListCfg::addColorHelper() {
	Model_Script* newScript = NULL;
	if (this->repository.getScriptByName("grub-customizer_menu_color_helper") == NULL) {
		Model_Script* newScript = this->repository.createScript("grub-customizer_menu_color_helper", this->env.cfg_dir + "06_grub-customizer_menu_color_helper", "#!/bin/sh\n\
\n\
if [ \"x${GRUB_BACKGROUND}\" != \"x\" ] ; then\n\
	if [ \"x${GRUB_COLOR_NORMAL}\" != \"x\" ] ; then\n\
	echo \"set color_normal=${GRUB_COLOR_NORMAL}\"\n\
	fi\n\
\n\
	if [ \"x${GRUB_COLOR_HIGHLIGHT}\" != \"x\" ] ; then\n\
	echo \"set color_highlight=${GRUB_COLOR_HIGHLIGHT}\"\n\
	fi\n\
fi\n\
");
		assert(newScript != NULL);
		Model_Proxy newProxy(*newScript);
		newProxy.index = 6;
		this->proxies.push_back(newProxy);
	}
}

std::list<Model_Entry*> Model_ListCfg::getRemovedEntries(Model_Entry* parent) {
	std::list<Model_Entry*> result;
	if (parent == NULL) {
		for (std::list<Model_Script>::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++) {
			std::list<Model_Entry*> subResult = this->getRemovedEntries(&iter->root);
			result.insert(result.end(), subResult.begin(), subResult.end());
		}
	} else {
		if (!this->proxies.getVisibleRuleForEntry(*parent)) {
			result.push_back(parent);
		}
		for (std::list<Model_Entry>::iterator entryIter = parent->subEntries.begin(); entryIter != parent->subEntries.end(); entryIter++) {
			std::list<Model_Entry*> subResult = this->getRemovedEntries(&*entryIter);
			result.insert(result.end(), subResult.begin(), subResult.end());
		}
	}
	return result;
}

Model_Rule* Model_ListCfg::addEntry(Model_Entry& entry) {
	Model_Script* sourceScript = this->repository.getScriptByEntry(entry);
	assert(sourceScript != NULL);

	Model_Proxy* targetProxy = NULL;
	if (this->proxies.size() && this->proxies.back().dataSource == sourceScript) {
		targetProxy = &this->proxies.back();
		targetProxy->set_isExecutable(true);
	} else {
		this->proxies.push_back(Model_Proxy(*sourceScript, false));
		targetProxy = &this->proxies.back();
		this->renumerate();
	}

	Model_Rule::RuleType type = Model_Rule::NORMAL;
	if (entry.type == Model_Entry::SUBMENU || entry.type == Model_Entry::SCRIPT_ROOT) {
		type = Model_Rule::OTHER_ENTRIES_PLACEHOLDER;
	} else if (entry.type == Model_Entry::PLAINTEXT) {
		type = Model_Rule::PLAINTEXT;
	}

	Model_Rule* rule = targetProxy->getRuleByEntry(entry, targetProxy->rules, type);
	Model_Rule ruleCopy = *rule;
	ruleCopy.isVisible = true;
	targetProxy->removeEquivalentRules(*rule);
	targetProxy->rules.push_back(ruleCopy);
	return &targetProxy->rules.back();
}

/**
 * deletes an entry and its rules
 */
void Model_ListCfg::deleteEntry(Model_Entry const& entry) {
	for (std::list<Model_Proxy>::iterator proxyIter = this->proxies.begin(); proxyIter != this->proxies.end(); proxyIter++) {
		Model_Rule* rule = proxyIter->getRuleByEntry(entry, proxyIter->rules, Model_Rule::NORMAL);
		if (rule) {
			proxyIter->removeRule(rule);
		}
	}
	this->repository.getScriptByEntry(entry)->deleteEntry(entry);
}

Model_ListCfg::operator ArrayStructure() const {
	ArrayStructure result;
	result["eventListener"] = this->eventListener;
	result["proxies"] = ArrayStructure(this->proxies);
	result["repository"] = ArrayStructure(this->repository);
	result["progress"] = this->progress;
	result["progress_name"] = this->progress_name;
	result["progress_pos"] = this->progress_pos;
	result["progress_max"] = this->progress_max;
	result["mutex"] = this->mutex;
	result["errorLogFile"] = this->errorLogFile;
	result["verbose"] = this->verbose;
	result["error_proxy_not_found"] = this->error_proxy_not_found;
	result["env"] = ArrayStructure(this->env);
	result["ignoreLock"] = this->ignoreLock;
	result["cancelThreadsRequested"] = this->cancelThreadsRequested;
	return result;
}






