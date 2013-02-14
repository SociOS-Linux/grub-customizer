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

ScriptSourceMap::ScriptSourceMap(Model_Env& env)
	: env(env)
{
}

std::string ScriptSourceMap::_getFilePath() {
	return this->env.cfg_dir + "/.script_sources.txt";
}

void ScriptSourceMap::load() {
	this->clear();

	FILE* file = fopen(this->_getFilePath().c_str(), "r");
	if (file) {
		CsvReader csv(file);
		std::map<std::string, std::string> dataRow;
		while ((dataRow = csv.read()).size()) {
			(*this)[dataRow["default_name"]] = dataRow["current_name"];
		}
		fclose(file);
	}
}

void ScriptSourceMap::save() {
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


