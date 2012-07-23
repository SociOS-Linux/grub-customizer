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

#include "grublistCfg.h"

GrublistCfg::GrublistCfg(GrubEnv& env)
 : error_proxy_not_found(false),
 progress(0),
 cancelThreadsRequested(false), verbose(true), env(env), eventListener(NULL),
 mutex(NULL), errorLogFile(ERROR_LOG_FILE), ignoreLock(false)
{}

void GrublistCfg::setEventListener(EventListener_model& eventListener) {
	this->eventListener = &eventListener;
}

void GrublistCfg::setMutex(Mutex& mutex) {
	this->mutex = &mutex;
}

void GrublistCfg::setLogger(Logger& logger) {
	this->CommonClass::setLogger(logger);
	this->proxies.setLogger(logger);
	this->repository.setLogger(logger);
}

void GrublistCfg::lock(){
	if (this->ignoreLock)
		return;
	if (this->mutex == NULL)
		throw MISSING_MUTEX;
	this->mutex->lock();
}
bool GrublistCfg::lock_if_free(){
	if (this->ignoreLock)
		return true;
	if (this->mutex == NULL)
		throw MISSING_MUTEX;
	return this->mutex->trylock();
}
void GrublistCfg::unlock(){
	if (this->ignoreLock)
		return;
	if (this->mutex == NULL)
		throw MISSING_MUTEX;
	this->mutex->unlock();
}

bool GrublistCfg::createScriptForwarder(std::string const& scriptName) const {
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

bool GrublistCfg::removeScriptForwarder(std::string const& scriptName) const {
	std::string scriptNameNoPath = scriptName.substr((this->env.cfg_dir+"/proxifiedScripts/").length());
	std::string filePath = this->env.cfg_dir+"/LS_"+scriptNameNoPath;
	return unlink(filePath.c_str()) == 0;
}

std::string GrublistCfg::readScriptForwarder(std::string const& scriptForwarderFilePath) const {
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

void GrublistCfg::load(bool preserveConfig){
	if (!preserveConfig){
		send_new_load_progress(0);

		DIR* hGrubCfgDir = opendir(this->env.cfg_dir.c_str());

		if (!hGrubCfgDir){
			throw GRUB_CFG_DIR_NOT_FOUND;
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
		while (entry = readdir(hGrubCfgDir)){
			stat((this->env.cfg_dir+"/"+entry->d_name).c_str(), &fileProperties);
			if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
				if (entry->d_name[2] == '_'){ //check whether it's an script (they should be named XX_scriptname)…
					this->proxies.push_back(Proxy());
					this->proxies.back().fileName = this->env.cfg_dir+"/"+entry->d_name;
					this->proxies.back().index = (entry->d_name[0]-'0')*10 + (entry->d_name[1]-'0');
					this->proxies.back().permissions = fileProperties.st_mode & ~S_IFMT;
				
					FILE* proxyFile = fopen((this->env.cfg_dir+"/"+entry->d_name).c_str(), "r");
					ProxyScriptData data(proxyFile);
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
	for (Repository::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++){
		if (iter->isInScriptDir(env.cfg_dir)){
			//createScriptForwarder & disable proxies
			createScriptForwarder(iter->fileName);
			std::list<Proxy*> relatedProxies = proxies.getProxiesByScript(*iter);
			for (std::list<Proxy*>::iterator piter = relatedProxies.begin(); piter != relatedProxies.end(); piter++){
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
		throw GRUB_CMD_EXEC_FAILED;
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
	for (Repository::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++){
		if (iter->isInScriptDir(env.cfg_dir)){
			//removeScriptForwarder & reset proxy permissions
			bool result = removeScriptForwarder(iter->fileName);
			if (!result) {
				this->log("removing of script forwarder not successful!", Logger::ERROR);
			}
		}
		std::list<Proxy*> relatedProxies = proxies.getProxiesByScript(*iter);
		for (std::list<Proxy*>::iterator piter = relatedProxies.begin(); piter != relatedProxies.end(); piter++){
			chmod((*piter)->fileName.c_str(), (*piter)->permissions);
		}
	}
	this->unlock();
	
	this->log("loading completed", Logger::EVENT);
	send_new_load_progress(1);
}


void GrublistCfg::readGeneratedFile(FILE* source, bool createScriptIfNotFound, bool createProxyIfNotFound){
	GrubConfRow row;
	Script* script;
	int i = 0;
	bool inScript = false;
	std::string plaintextBuffer = "";
	while (!cancelThreadsRequested && (row = GrubConfRow(source))){
		if (!inScript && row.text.substr(0,10) == ("### BEGIN ") && row.text.substr(row.text.length()-4,4) == " ###"){
			this->lock();
			if (script) {
				if (plaintextBuffer != "") {
					Entry newEntry("#text", "", plaintextBuffer, Entry::PLAINTEXT);
					if (this->hasLogger()) {
						newEntry.setLogger(this->getLogger());
					}
					script->push_front(newEntry);
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
				this->proxies.push_back(Proxy(*script));
			}
			this->unlock();
			if (script){
				this->send_new_load_progress(0.1 + (0.7 / this->repository.size() * ++i));
			}
			inScript = true;
		} else if (inScript && row.text.substr(0,8) == ("### END ") && row.text.substr(row.text.length()-4,4) == " ###") {
			inScript = false;
		} else if (script != NULL && row.text.substr(0, 10) == "menuentry ") {
			this->lock();
			Entry newEntry(source, row, this->getLoggerPtr());
			script->push_back(newEntry);
			this->proxies.sync_all(false, false, script);
			this->unlock();
			this->send_new_load_progress(0.1 + (0.7 / this->repository.size() * i));
		} else if (script != NULL && row.text.substr(0, 8) == "submenu ") {
			this->lock();
			Entry newEntry(source, row, this->getLoggerPtr());
			script->push_back(newEntry);
			this->proxies.sync_all(false, false, script);
			this->unlock();
			this->send_new_load_progress(0.1 + (0.7 / this->repository.size() * i));
		} else if (inScript) { //Plaintext
			plaintextBuffer += row.text + "\n";
		}
	}
	this->lock();
	if (script) {
		if (plaintextBuffer != "") {
			Entry newEntry("#text", "", plaintextBuffer, Entry::PLAINTEXT);
			if (this->hasLogger()) {
				newEntry.setLogger(this->getLogger());
			}
			script->push_front(newEntry);
		}
		this->proxies.sync_all(true, true, script);
	}

	// sync all (including foreign entries)
	this->proxies.sync_all(true, true, NULL, this->repository.getScriptPathMap());

	this->unlock();
}

void GrublistCfg::save(){
	send_new_save_progress(0);
	std::map<std::string, int> samename_counter;
	proxies.deleteAllProxyscriptFiles();  //delete all proxies to get a clean file system
	proxies.clearTrash(); //delete all files of removed proxies
	for (Repository::iterator script_iter = repository.begin(); script_iter != repository.end(); script_iter++)
		script_iter->moveToBasedir(this->env.cfg_dir);
	
	send_new_save_progress(0.1);

	int mkdir_result = mkdir((this->env.cfg_dir+"/proxifiedScripts").c_str(), 0755); //create this directory if it doesn't allready exist

	// get new script locations
	std::map<Script const*, std::string> scriptTargetMap; // scripts and their target directories
	for (Repository::iterator script_iter = repository.begin(); script_iter != repository.end(); script_iter++) {
		std::list<Proxy*> relatedProxies = proxies.getProxiesByScript(*script_iter);
		if (proxies.proxyRequired(*script_iter)){
			scriptTargetMap[&*script_iter] = this->env.cfg_dir+"/proxifiedScripts/"+pscriptname_encode(script_iter->name, samename_counter[script_iter->name]++);
		} else {
			std::ostringstream nameStream;
			nameStream << std::setw(2) << std::setfill('0') << relatedProxies.front()->index << "_" << script_iter->name;
			std::list<Proxy*> relatedProxies = proxies.getProxiesByScript(*script_iter);
			scriptTargetMap[&*script_iter] = this->env.cfg_dir+"/"+nameStream.str();
		}
	}

	// move scripts and create proxies
	int proxyCount = 0;
	for (Repository::iterator script_iter = repository.begin(); script_iter != repository.end(); script_iter++){
		std::list<Proxy*> relatedProxies = proxies.getProxiesByScript(*script_iter);
		if (proxies.proxyRequired(*script_iter)){
			script_iter->moveFile(scriptTargetMap[&*script_iter], 0755);
			for (std::list<Proxy*>::iterator proxy_iter = relatedProxies.begin(); proxy_iter != relatedProxies.end(); proxy_iter++){
				std::map<Entry const*, Script const*> entrySourceMap = this->getEntrySources(**proxy_iter);
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
		while (entry = readdir(hScriptDir)){
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

std::map<Entry const*, Script const*> GrublistCfg::getEntrySources(Proxy const& proxy, Rule const* parent) const {
	std::list<Rule> const& list = parent ? parent->subRules : proxy.rules;
	std::map<Entry const*, Script const*> result;
	assert(proxy.dataSource != NULL);
	for (std::list<Rule>::const_iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->dataSource && !proxy.ruleIsFromOwnScript(*iter)) {
			Script const* script = this->repository.getScriptByEntry(*iter->dataSource);
			if (script != NULL) {
				result[iter->dataSource] = script;
			}
		} else if (iter->type == Rule::SUBMENU) {
			std::map<Entry const*, Script const*> subResult = this->getEntrySources(proxy, &*iter);
			if (subResult.size()) {
				result.insert(subResult.begin(), subResult.end());
			}
		}
	}
	return result;
}

bool GrublistCfg::loadStaticCfg(){
	FILE* oldConfigFile = fopen(env.output_config_file.c_str(), "r");
	if (oldConfigFile){
		this->readGeneratedFile(oldConfigFile, true, true);
		fclose(oldConfigFile);
		return true;
	}
	return false;
}

void GrublistCfg::renameRule(Rule* rule, std::string const& newName){
	rule->outputName = newName;
}

std::string GrublistCfg::getGrubErrorMessage() const {
	FILE* errorLogFile = fopen(this->errorLogFile.c_str(), "r");
	std::string errorMessage;
	int c;
	while ((c = fgetc(errorLogFile)) != EOF) {
		errorMessage += char(c);
	}
	fclose(errorLogFile);
	return errorMessage;
}

bool GrublistCfg::compare(GrublistCfg const& other) const {
	std::list<const Rule*> rlist[2];
	for (int i = 0; i < 2; i++){
		const GrublistCfg* gc = i == 0 ? this : &other;
		for (ProxyList::const_iterator piter = gc->proxies.begin(); piter != gc->proxies.end(); piter++){
			assert(piter->dataSource != NULL);
			if (piter->isExecutable() && piter->dataSource){
				if (piter->dataSource->fileName == "") { // if the associated file isn't found
					return false;
				}
				std::string fname = piter->dataSource->fileName.substr(other.env.cfg_dir.length()+1);
				if (i == 0 || fname[0] >= '1' && fname[0] <= '9' && fname[1] >= '0' && fname[1] <= '9' && fname[2] == '_'){
					std::list<Rule const*> comparableRules = this->getComparableRules(piter->rules);
					rlist[i].splice(rlist[i].end(), comparableRules);
				}
			}
		}
	}
	return GrublistCfg::compareLists(rlist[0], rlist[1]);
}

std::list<Rule const*> GrublistCfg::getComparableRules(std::list<Rule> const& list) {
	std::list<Rule const*> result;
	for (std::list<Rule>::const_iterator riter = list.begin(); riter != list.end(); riter++){
		if ((riter->type == Rule::NORMAL && riter->dataSource || riter->type == Rule::SUBMENU) && riter->isVisible){
			result.push_back(&*riter);
		}
	}
	return result;
}

bool GrublistCfg::compareLists(std::list<Rule const*> a, std::list<Rule const*> b) {
	if (a.size() != b.size())
		return false;

	std::list<const Rule*>::iterator self_iter = a.begin(), other_iter = b.begin();
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
		if ((*self_iter)->type == Rule::SUBMENU && !GrublistCfg::compareLists(GrublistCfg::getComparableRules((*self_iter)->subRules), GrublistCfg::getComparableRules((*other_iter)->subRules))) {
			return false;
		}
		self_iter++;
		other_iter++;
	}
	return true;
}


void GrublistCfg::send_new_load_progress(double newProgress){
	if (this->eventListener != NULL){
		this->progress = newProgress;
		this->eventListener->loadProgressChanged();
	}
	else if (this->verbose) {
		this->log("cannot show updated load progress - no UI connected!", Logger::ERROR);
	}
}

void GrublistCfg::send_new_save_progress(double newProgress){
	if (this->eventListener != NULL){
		this->progress = newProgress;
		this->eventListener->saveProgressChanged();
	}
	else if (this->verbose) {
		this->log("cannot show updated save progress - no UI connected!", Logger::ERROR);
	}
}

void GrublistCfg::cancelThreads(){
	cancelThreadsRequested = true;
}


void GrublistCfg::reset(){
	this->lock();
	this->repository.clear();
	this->proxies.clear();
	this->unlock();
}

double GrublistCfg::getProgress() const {
	return progress;
}

void GrublistCfg::renumerate(){
	short int i = 10;
	for (ProxyList::iterator iter = this->proxies.begin(); iter != this->proxies.end(); iter++){
		iter->index = i++;
	}
	this->proxies.sort();
}

Rule& GrublistCfg::moveRule(Rule* rule, int direction){
	try {
		return this->proxies.getProxyByRule(rule)->moveRule(rule, direction);
	} catch (Proxy::Exception e) {
		if (e == Proxy::NO_MOVE_TARGET_FOUND) {
			Proxy* currentProxy = this->proxies.getProxyByRule(rule);
			std::list<Proxy>::iterator proxyIter = this->proxies.begin();
			for (;proxyIter != this->proxies.end() && &*proxyIter != currentProxy; proxyIter++) {}

			if (direction == -1 && proxyIter != this->proxies.begin()) {
				proxyIter--;
				proxyIter->rules.push_back(*rule);
				if (currentProxy->ruleIsFromOwnScript(*rule)) {
					rule->isVisible = false;
				} else {
					currentProxy->rules.pop_front();
				}
				return proxyIter->rules.back();
			} else if (direction == 1 && proxyIter != this->proxies.end() && &*proxyIter != &this->proxies.back()) {
				proxyIter++;
				proxyIter->rules.push_front(*rule);
				if (currentProxy->ruleIsFromOwnScript(*rule)) {
					rule->isVisible = false;
				} else {
					currentProxy->rules.pop_back();
				}
				return proxyIter->rules.front();
			} else {
				throw GrublistCfg::NO_MOVE_TARGET_FOUND;
			}
		} else {
			throw e;
		}
	}
}

void GrublistCfg::swapProxies(Proxy* a, Proxy* b){
	int index1 = a->index;
	a->index = b->index;
	b->index = index1;
	this->proxies.sort();
}

Rule* GrublistCfg::createSubmenu(Rule* child) {
	return this->proxies.getProxyByRule(child)->createSubmenu(child);
}

Rule* GrublistCfg::removeSubmenu(Rule* child) {
	return this->proxies.getProxyByRule(child)->removeSubmenu(child);
}

bool GrublistCfg::cfgDirIsClean(){
	DIR* hGrubCfgDir = opendir(this->env.cfg_dir.c_str());
	if (hGrubCfgDir){
		struct dirent *entry;
		struct stat fileProperties;
		while (entry = readdir(hGrubCfgDir)){
			std::string fname = entry->d_name;
			if (fname.length() >= 4 && fname.substr(0,3) == "LS_" || fname.substr(0,3) == "PS_" || fname.substr(0,3) == "DS_")
				return false;
		}
		closedir(hGrubCfgDir);
	}
	return true;
}
void GrublistCfg::cleanupCfgDir(){
	this->log("cleaning up cfg dir!", Logger::IMPORTANT_EVENT);
	
	DIR* hGrubCfgDir = opendir(this->env.cfg_dir.c_str());
	if (hGrubCfgDir){
		struct dirent *entry;
		struct stat fileProperties;
		std::list<std::string> lsfiles, dsfiles, psfiles;
		std::list<std::string> proxyscripts;
		while (entry = readdir(hGrubCfgDir)){
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

void GrublistCfg::addColorHelper() {
	Script* newScript = NULL;
	if (this->repository.getScriptByName("grub-customizer_menu_color_helper") == NULL) {
		Script* newScript = this->repository.createScript("grub-customizer_menu_color_helper", this->env.cfg_dir + "06_grub-customizer_menu_color_helper", "#!/bin/sh\n\
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
		Proxy newProxy(*newScript);
		newProxy.index = 6;
		this->proxies.push_back(newProxy);
	}
}



