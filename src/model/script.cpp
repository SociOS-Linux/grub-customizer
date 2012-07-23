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
	: name(name), fileName(fileName), root("DUMMY", "DUMMY", "DUMMY", Entry::SCRIPT_ROOT), isCustomScript(false)
{
	FILE* script = fopen(fileName.c_str(), "r");
	if (script) {
		GrubConfRow row1(script), row2(script);
		if (row1.text == CUSTOM_SCRIPT_SHEBANG && row2.text == CUSTOM_SCRIPT_PREFIX) {
			isCustomScript = true;
		}
		fclose(script);
	}
}

bool Script::isModified(Entry* parent) {
	if (!parent) {
		parent = &this->root;
	}
	for (std::list<Entry>::iterator iter = parent->subEntries.begin(); iter != parent->subEntries.end(); iter++) {
		if (iter->isModified) {
			return true;
		} else if (iter->type == Entry::SUBMENU) {
			bool modified = this->isModified(&*iter);
			if (modified) {
				return true;
			}
		}
	}
	return false;
}

std::list<Entry>& Script::entries() {
	return this->root.subEntries;
}

std::list<Entry> const& Script::entries() const {
	return this->root.subEntries;
}

bool Script::isInScriptDir(std::string const& cfg_dir) const {
	return this->fileName.substr(cfg_dir.length(), std::string("/proxifiedScripts/").length()) == "/proxifiedScripts/";
}
Entry* Script::getEntryByPath(std::list<std::string> const& path){
	Entry* result = NULL;
	if (path.size() == 0) { // top level oep
		result = &this->root;
	} else {
		for (std::list<std::string>::const_iterator iter = path.begin(); iter != path.end(); iter++) {
			result = this->getEntryByName(*iter, result != NULL ? result->subEntries : this->entries());
			if (result == NULL)
				return NULL;
		}
	}
	return result;
}

Entry* Script::getEntryByName(std::string const& name, std::list<Entry>& parentList) {
	for (std::list<Entry>::iterator iter = parentList.begin(); iter != parentList.end(); iter++){
		if (iter->name == name)
			return &*iter;
	}
	return NULL;
}

Entry* Script::getEntryByHash(std::string const& hash, std::list<Entry>& parentList) {
	for (std::list<Entry>::iterator iter = parentList.begin(); iter != parentList.end(); iter++){
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
	if (&entry == &this->root) { // return an empty list if it's the root entry!
		return std::list<std::string>();
	}
	std::list<Entry> const& list = parent ? parent->subEntries : this->entries();
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
		result += result.size() ? "/*" : "*";
	}
	return result;
}

bool Script::hasEntry(Entry const& entry, Entry const * parent) const {
	if (parent == NULL && &this->root == &entry) { // check toplevel entry
		return true;
	}

	std::list<Entry> const& list = parent ? parent->subEntries : this->entries();

	for (std::list<Entry>::const_iterator iter = list.begin(); iter != list.end(); iter++) {
		if (&*iter == &entry) {
			return true;
		}
		if (iter->type == Entry::SUBMENU) {
			bool has = this->hasEntry(entry, &*iter);
			if (has) {
				return true;
			}
		}
	}
	return false;
}





