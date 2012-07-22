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

Rule::Rule(Entry& source, bool isVisible, std::list<std::list<std::string> > const& pathesToIgnore, std::list<std::string> const& currentPath) //generate rule for given entry. __idname is only required for re-syncing (soft-reload)
	: type(Rule::NORMAL), isVisible(isVisible), __idname(source.name), outputName(source.name), dataSource(&source)
{
	if (source.type == Entry::SUBMENU) {
		this->subRules.push_front(Rule(Rule::OTHER_ENTRIES_PLACEHOLDER, "*", true));
	}
	for (std::list<Entry>::iterator iter = source.subEntries.begin(); iter != source.subEntries.end(); iter++) {
		std::list<std::string> currentPath_in_loop = currentPath;
		currentPath_in_loop.push_back(iter->name);

		//find out if currentPath is on the blacklist
		bool currentPath_in_loop_is_blacklisted = false;
		for (std::list<std::list<std::string> >::const_iterator pti_iter = pathesToIgnore.begin(); pti_iter != pathesToIgnore.end(); pti_iter++) {
			if (*pti_iter == currentPath_in_loop) {
				currentPath_in_loop_is_blacklisted = true;
				break;
			}
		}

		//add this entry as rule if not blacklisted
		if (!currentPath_in_loop_is_blacklisted){
			this->subRules.push_back(Rule(*iter, isVisible, pathesToIgnore, currentPath_in_loop));
		}
	}
}

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

	if (this->subRules.size() > 0) {
		result += "{";
		for (std::list<Rule>::iterator iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
			if (iter != this->subRules.begin())
				result += ", ";
			result += std::string(*iter);
		}
		result += "}";
	}
	return result;
}

Rule::Rule(RuleType type, std::string name, bool isVisible)
	: type(type), isVisible(isVisible), __idname(name), outputName(name), dataSource(NULL)
{}

Rule::Rule(RuleType type, std::list<std::string> path, bool isVisible)
	: type(type), isVisible(isVisible), __idpath(path), outputName(path.back()), dataSource(NULL)
{}

std::string Rule::getEntryName() const {
	if (this->dataSource)
		return this->dataSource->name;
	else
		return "?";
}


void Rule::print() const {
	if (this->isVisible && this->dataSource){
		std::cout << "menuentry \""+this->outputName+"\""+this->dataSource->extension+"{\n";
		if (this->subRules.size() == 0) {
			std::cout << this->dataSource->content;
		} else {
			for (std::list<Rule>::const_iterator iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
				iter->print();
			}
		}
		std::cout << "}\n";
	}
}
