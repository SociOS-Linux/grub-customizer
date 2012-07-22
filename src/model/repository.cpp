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

#include "repository.h"

void Repository::load(std::string const& directory, bool is_proxifiedScript_dir){
	DIR* dir = opendir(directory.c_str());
	if (dir){
		struct dirent *entry;
		struct stat fileProperties;
		while (entry = readdir(dir)){
			stat((directory+"/"+entry->d_name).c_str(), &fileProperties);
			if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
				if (!is_proxifiedScript_dir && !ProxyScriptData::is_proxyscript(directory+"/"+entry->d_name) && std::string(entry->d_name).length() >= 4 && entry->d_name[0] >= '1' && entry->d_name[0] <= '9' && entry->d_name[1] >= '0' && entry->d_name[1] <= '9' && entry->d_name[2] == '_'){
					this->push_back(Script(std::string(entry->d_name).substr(3), directory+"/"+entry->d_name));
				}
				else if (is_proxifiedScript_dir) {
					this->push_back(Script(pscriptname_decode(entry->d_name), directory+"/"+entry->d_name));
				}
			}
		}
		closedir(dir);
	}
}

Script* Repository::getScriptByFilename(std::string const& fileName, bool createScriptIfNotFound) {
	Script* result = NULL;
	for (Repository::iterator iter = this->begin(); iter != this->end() && !result; iter++){
		if (iter->fileName == fileName)
			result = &*iter;
	}
	if (result == NULL && createScriptIfNotFound){
		this->push_back(Script("noname", fileName));
		result = &this->back();
	}
	return result;
}

Script* Repository::getNthScript(int pos){
	Script* result = NULL;
	int i = 0;
	for (Repository::iterator iter = this->begin(); result == NULL && iter != this->end(); iter++){
		if (i == pos)
			result = &*iter;
		i++;
	}
	return result;
}

void Repository::deleteAllEntries(){
	for (Repository::iterator iter = this->begin(); iter != this->end(); iter++){
		iter->clear();
	}
}

