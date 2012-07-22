#include "script.h"

Script::Script(std::string const& name, std::string const& fileName)
	: name(name), fileName(fileName)
{}
bool Script::isInScriptDir(std::string const& cfg_dir) const {
	return this->fileName.substr(cfg_dir.length(), std::string("/proxifiedScripts/").length()) == "/proxifiedScripts/";
}
Entry* Script::getEntryByName(std::string const& name){
	Entry* result = NULL;
	for (Script::iterator iter = this->begin(); iter != this->end() && !result; iter++){
		if (iter->name == name)
			result = &*iter;
	}
	return result;
}

void Script::moveToBasedir(std::string const& cfg_dir){
	std::string newPath;
	if (isInScriptDir(cfg_dir)){
		newPath = cfg_dir+"/PS_"+this->fileName.substr((cfg_dir+"/proxifiedScripts/").length());
	}
	else {
		newPath = cfg_dir+"/DS_"+this->fileName.substr(cfg_dir.length()+1);
	}
	int renameSuccess = rename(this->fileName.c_str(), newPath.c_str());
	if (renameSuccess == 0)
		this->fileName = newPath;
}

bool Script::moveFile(std::string const& newPath, short int permissions){
	int rename_success = rename(this->fileName.c_str(), newPath.c_str());
	if (rename_success == 0){
		this->fileName = newPath;
		if (permissions != -1)
			chmod(this->fileName.c_str(), permissions);
		return true;
	}
	return false;
}
