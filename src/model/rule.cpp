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

#include "rule.h"

std::string str_replace(const std::string &search, const std::string &replace, std::string subject) {
	size_t pos = 0;
	while (pos < subject.length() && (pos = subject.find(search, pos)) != -1){
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

Rule::Rule(Entry& source, bool isVisible) //generate rule for given entry. __idname is only required for re-syncing (soft-reload)
	: type(Rule::NORMAL), isVisible(isVisible), __idname(source.name), outputName(source.name), dataSource(&source)
{}

Rule::operator std::string(){
	std::string result = isVisible ? "+" : "-";
	if (dataSource)
		result += "'"+str_replace("'", "''", dataSource->name)+"'";
	else if (type == OTHER_ENTRIES_PLACEHOLDER)
		result += "*";
	else
		result += "???";
	if (type == NORMAL && (dataSource && dataSource->name != outputName))
		result += " as '"+str_replace("'", "''", outputName)+"'";
	return result;
}

Rule::Rule(RuleType type, std::string name, bool isVisible)
	: type(type), isVisible(isVisible), __idname(name), outputName(name), dataSource(NULL)
{}

std::string Rule::getEntryName() const {
	if (this->dataSource)
		return this->dataSource->name;
	else
		return "?";
}


void Rule::print() const {
	if (this->isVisible && this->dataSource){
		std::cout << "menuentry \""+this->outputName+"\""+this->dataSource->extension+"{\n"+this->dataSource->content+"}\n";
	}
}
