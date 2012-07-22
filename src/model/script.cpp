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
Entry* Script::getEntryByPath(std::list<std::string> const& path){
	Entry* result = NULL;
	for (std::list<std::string>::const_iterator iter = path.begin(); iter != path.end(); iter++) {
		result = this->getEntryByName(*iter, result != NULL ? result->subEntries : *this);
		if (result == NULL)
			return NULL;
	}
	return result;
}

/**
 * find an entry by its name. If there are zero or multiple entries having the specified name,
 * NULL is returned
 */
Entry* Script::getEntryByName(std::string const& name, std::list<Entry>& parentList) {
	Entry* result = NULL;
	for (Script::iterator iter = parentList.begin(); iter != parentList.end(); iter++){
		if (iter->name == name) {
			if (result != NULL) {
				return NULL; // entry name not unique!
			}
			result = &*iter;
		}
	}
	return result;
}

Entry* Script::getEntryByHash(std::string const& hash, std::list<Entry>& parentList) {
	for (Script::iterator iter = parentList.begin(); iter != parentList.end(); iter++){
		if (iter->type == Entry::MENUENTRY && iter->content != "" && md5(iter->content) == hash) {
			return &*iter;
		} else if (iter->type == Entry::SUBMENU) {
			Entry* result = this->getEntryByHash(hash, iter->subEntries);
			if (result != NULL) {
				return result;
			}
		}
	}
	return NULL;
}

std::list<Entry>* Script::getListByPath(std::list<std::string> const& path) {
	if (path.size()) {
		Entry* e = this->getEntryByPath(path);
		if (e == NULL) {
			return NULL;
		} else {
			return &e->subEntries;
		}
	} else {
		return this;
	}
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

std::list<std::string> Script::buildPath(Entry const& entry, Entry const* parent) const {
	std::list<Entry> const& list = parent ? parent->subEntries : *this;
	for (std::list<Entry>::const_iterator iter = list.begin(); iter != list.end(); iter++) {
		if (&*iter == &entry) {
			std::list<std::string> result;
			result.push_back(iter->name);
			return result;
		}
		if (iter->type == Entry::SUBMENU) {
			try {
				std::list<std::string> result = this->buildPath(entry, &*iter);
				result.push_front(iter->name);
				return result;
			} catch (Script::Exception e) {
				//continue
			}
		}
	}
	throw ELEMENT_NOT_FOUND;
}


std::list<std::string> Script::buildPath(Entry const& entry) const {
	return this->buildPath(entry, NULL);
}

std::string Script::buildPathString(Entry const& entry, bool withOtherEntriesPlaceholder) const {
	std::string result;
	std::list<std::string> list = this->buildPath(entry, NULL);
	for (std::list<std::string>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (result != "") {
			result += "/";
		}
		result += "'"+str_replace("'", "''", *iter)+"'";
	}

	if (withOtherEntriesPlaceholder) {
		result += "/*";
	}
	return result;
}


