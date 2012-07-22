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

#include "script.h"

Script::Script(std::string const& name, std::string const& fileName)
	: name(name), fileName(fileName)
{}
bool Script::isInScriptDir(std::string const& cfg_dir) const {
	return this->fileName.substr(cfg_dir.length(), std::string("/proxifiedScripts/").length()) == "/proxifiedScripts/";
}
Entry* Script::getEntryByName(std::string const& name){
	return this->getEntryByName(name, *this);
}

Entry* Script::getEntryByName(std::string const& name, std::list<Entry>& parentList) {
	for (Script::iterator iter = parentList.begin(); iter != parentList.end(); iter++){
		if (iter->name == name)
			return &*iter;
		if (iter->subEntries.size()) {
			Entry* result = this->getEntryByName(name, iter->subEntries);
			if (result != NULL)
				return result;
		}
	}
	return NULL;
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
