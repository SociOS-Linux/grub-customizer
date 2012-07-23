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

Rule::Rule(Entry& source, bool isVisible, EntryPathFollower& pathFollower, std::list<std::list<std::string> > const& pathesToIgnore, std::list<std::string> const& currentPath) //generate rule for given entry. __idname is only required for re-syncing (soft-reload)
	: type(source.type == Entry::PLAINTEXT ? Rule::PLAINTEXT : (source.type == Entry::SUBMENU ? Rule::SUBMENU : Rule::NORMAL)), isVisible(isVisible), __idpath(currentPath), outputName(source.name), dataSource(source.type == Entry::SUBMENU ? NULL : &source)
{
	if (source.type == Entry::SUBMENU) {
		Rule placeholder(Rule::OTHER_ENTRIES_PLACEHOLDER, currentPath, "*", true);
		placeholder.dataSource = pathFollower.getEntryByPath(currentPath);
		placeholder.dataSource_list = pathFollower.getListByPath(currentPath);
		this->subRules.push_front(placeholder);
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
			this->subRules.push_back(Rule(*iter, isVisible, pathFollower, pathesToIgnore, currentPath_in_loop));
		}
	}
}

std::string Rule::toString(EntryPathBilder const& pathBuilder){
	std::string result = isVisible ? "+" : "-";
	if (type == Rule::PLAINTEXT) {
		result += "#text";
	} else if (dataSource) {
		result += pathBuilder.buildPathString(*dataSource, this->type == OTHER_ENTRIES_PLACEHOLDER);
		result += "~" + md5(this->dataSource->content) + "~";
	} else if (type == Rule::OTHER_ENTRIES_PLACEHOLDER) {
		result += "*"; //root level placeholders
	} else if (type == Rule::SUBMENU) {
		result += "'SUBMENU'"; // dummy data source
	} else {
		result += "???";
	}
	if (type == Rule::SUBMENU || (type == Rule::NORMAL && dataSource && dataSource->name != outputName)) {
		result += " as '"+str_replace("'", "''", outputName)+"'";
	}

	std::string sourceScriptPath = pathBuilder.buildScriptPath(*dataSource);
	if (sourceScriptPath != "") {
		result += " from '" + sourceScriptPath + "'";
	}

	if (type == Rule::SUBMENU && this->subRules.size() > 0) {
		result += "{";
		for (std::list<Rule>::iterator iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
			if (iter != this->subRules.begin())
				result += ", ";
			result += iter->toString(pathBuilder);
		}
		result += "}";
	}
	return result;
}

Rule::Rule(RuleType type, std::list<std::string> path, std::string outputName, bool isVisible)
	: type(type), isVisible(isVisible), __idpath(path), outputName(outputName), dataSource(NULL)
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

bool Rule::hasRealSubrules() const {
	for (std::list<Rule>::const_iterator iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
		if (iter->isVisible && ((iter->type == Rule::NORMAL && iter->dataSource) || (iter->type == Rule::SUBMENU && iter->hasRealSubrules()))) {
			return true;
		}
	}
	return false;
}


void Rule::print() const {
	if (this->isVisible) {
		if (this->type == Rule::PLAINTEXT && this->dataSource) {
			std::cout << this->dataSource->content;
		} else if (this->type == Rule::NORMAL && this->dataSource) {
			std::cout << "menuentry";
			std::cout << " \"" << this->outputName << "\"" << this->dataSource->extension << "{\n";
			std::cout << this->dataSource->content;
			std::cout << "}\n";
		} else if (this->type == Rule::SUBMENU && this->hasRealSubrules()) {
			std::cout << "submenu" << " \"" << this->outputName << "\"" << "{\n";
			for (std::list<Rule>::const_iterator iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
				iter->print();
			}
			std::cout << "}\n";
		}
	}
}
