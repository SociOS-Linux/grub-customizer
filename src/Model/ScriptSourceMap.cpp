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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "ScriptSourceMap.h"

Model_ScriptSourceMap::Model_ScriptSourceMap(Model_Env& env)
	: env(env)
{
}

std::string Model_ScriptSourceMap::_getFilePath() {
	return this->env.cfg_dir + "/.script_sources.txt";
}

void Model_ScriptSourceMap::load() {
	this->clear();

	FILE* file = fopen(this->_getFilePath().c_str(), "r");
	if (file) {
		CsvReader csv(file);
		std::map<std::string, std::string> dataRow;
		while ((dataRow = csv.read()).size()) {
			(*this)[dataRow["default_name"]] = dataRow["current_name"];
		}
		fclose(file);

		if (this->size() == 1) {
			std::map<std::string, std::string>::iterator iter = this->begin();
			std::cout << "load: the only element is: " << iter->first << ": " << iter->second << std::endl;
		}
	}
}

void Model_ScriptSourceMap::registerMove(std::string const& sourceName, std::string const& destinationName) {
	if (sourceName == destinationName) {
		return;
	}
	if (this->has(destinationName)) { // script has been renamed to default name
		this->erase(destinationName);
		return;
	}

	std::string originalSourceName = this->getSourceName(sourceName);
	if (originalSourceName != "") { // update existing script entry
		(*this)[originalSourceName] = destinationName;
	} else {
		(*this)[sourceName] = destinationName;
	}
}

void Model_ScriptSourceMap::save() {
	FILE* file = fopen(this->_getFilePath().c_str(), "w");
	assert(file != NULL);
	CsvWriter csv(file);
	for (std::map<std::string, std::string>::iterator iter = this->begin(); iter != this->end(); iter++) {
		std::map<std::string, std::string> dataRow;
		dataRow["default_name"] = iter->first;
		dataRow["current_name"] = iter->second;
		csv.write(dataRow);
	}
	fclose(file);
}

bool Model_ScriptSourceMap::has(std::string const& sourceName) {
	return this->find(sourceName) != this->end();
}

std::string Model_ScriptSourceMap::getSourceName(std::string const& destinationName) {
	for (std::map<std::string, std::string>::iterator iter = this->begin(); iter != this->end(); iter++) {
		if (iter->second == destinationName) {
			return iter->first;
		}
	}
	return "";
}



