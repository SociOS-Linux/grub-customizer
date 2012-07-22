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

#include "proxy.h"

Proxy::Proxy()
	: dataSource(NULL), permissions(0755), index(90)
{	
}

Proxy::Proxy(Script& dataSource)
	: dataSource(&dataSource), permissions(0755), index(90)
{
	rules.push_back(Rule(Rule::OTHER_ENTRIES_PLACEHOLDER, std::list<std::string>(), "*", true));
	sync(true, true);
}

std::list<Rule> Proxy::parseRuleString(const char** ruleString) {
	std::list<Rule> rules;

	bool inString = false, inAlias = false;
	std::string name;
	std::list<std::string> path;
	bool visible = false;
	const char* iter = NULL;
	for (iter = *ruleString; *iter && (*iter != '}' || inString || inAlias); iter++) {
		if (!inString && *iter == '+')
			visible = true;
		else if (!inString && *iter == '-')
			visible = false;
		else if (*iter == '\'' && iter[1] != '\''){
			inString = !inString;
			if (iter[1] != '/') {
				if (!inString){
					if (inAlias)
						rules.back().outputName = name;
					else {
						path.push_back(name);
						rules.push_back(Rule(Rule::NORMAL, path, visible));
						path.clear();
					}
					inAlias = false;
				}
				name = "";
			}
		}
		else if (!inString && *iter == '*') {
			rules.push_back(Rule(Rule::OTHER_ENTRIES_PLACEHOLDER, path, "*", visible));
			path.clear();
		}
		else if (inString){
			name += *iter;
			if (*iter == '\'')
				iter++;
		}
		else if (!inString && *iter == 'a' && *++iter == 's')
			inAlias = true;
		else if (!inString && !inAlias && *iter == '/'){
			path.push_back(name);
			name = "";
		} else if (!inString && !inAlias && *iter == '{'){
			iter++;
			rules.back().subRules = Proxy::parseRuleString(&iter);
		}
	}
	*ruleString = iter;
	return rules;
}

void Proxy::importRuleString(const char* ruleString){
	rules = Proxy::parseRuleString(&ruleString);
}

Rule* Proxy::getRuleByEntry(Entry const& entry, std::list<Rule>& list) {
//	std::cout << "rule count: " << list.size() << std::endl;
	for (std::list<Rule>::iterator rule_iter = list.begin(); rule_iter != list.end(); rule_iter++){
		if (&entry == rule_iter->dataSource)
			return &*rule_iter;
		else {
//			std::cout << entry.name << " != " << rule_iter->__idname << std::endl;
			Rule* result = this->getRuleByEntry(entry, rule_iter->subRules);
			if (result)
				return result;
		}
	}
	return NULL;
}

bool Proxy::sync(bool deleteInvalidRules, bool expand){
	if (this->dataSource){
		this->sync_connectExisting();
		if (expand) {
			this->sync_add_placeholders();
			this->sync_expand();
		}

		if (deleteInvalidRules)
			this->sync_cleanup();

		return true;
	}
	else
		return false;
}

void Proxy::sync_connectExisting(Rule* parent) {
	this->__idPathList.clear();
	std::list<Rule>& list = parent ? parent->subRules : this->rules;
	for (std::list<Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		std::list<std::string> path = iter->__idpath;

		if (iter->type != Rule::OTHER_ENTRIES_PLACEHOLDER) {
			this->__idPathList.push_back(path);
		} else {
			this->__idPathList_OtherEntriesPlaceHolders.push_back(path);
		}
		iter->dataSource = this->dataSource->getEntryByPath(path);

		if (iter->type == Rule::OTHER_ENTRIES_PLACEHOLDER) {
			iter->dataSource_list = this->dataSource->getListByPath(path);
		}


		if (iter->subRules.size()) {
			this->sync_connectExisting(&*iter);
		}
	}
}

void Proxy::sync_add_placeholders(Rule* parent) {
	assert(parent == NULL || parent->dataSource != NULL);

	std::list<std::string> path = parent ? this->dataSource->buildPath(*parent->dataSource) : std::list<std::string>();
	//find out if currentPath is on the blacklist
	bool eop_is_blacklisted = false;

	for (std::list<std::list<std::string> >::const_iterator pti_iter = this->__idPathList_OtherEntriesPlaceHolders.begin(); pti_iter != this->__idPathList_OtherEntriesPlaceHolders.end(); pti_iter++) {
		if (*pti_iter == path) {
			eop_is_blacklisted = true;
			break;
		}
	}

	std::list<Rule>& list = parent ? parent->subRules : this->rules;
	if (!eop_is_blacklisted) {
		Rule newRule(Rule::OTHER_ENTRIES_PLACEHOLDER, parent ? this->dataSource->buildPath(*parent->dataSource) : std::list<std::string>(), "*", true);
		newRule.dataSource = this->dataSource->getEntryByPath(path);
		newRule.dataSource_list = this->dataSource->getListByPath(path);
		list.push_front(newRule);
		this->__idPathList_OtherEntriesPlaceHolders.push_back(path);
	}

	//sub entries (recursion)
	for (std::list<Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->dataSource && iter->type == Rule::NORMAL && iter->dataSource->type == Entry::SUBMENU) {
			this->sync_add_placeholders(&*iter);
		}
	}
}


void Proxy::sync_expand() {
	assert(this->dataSource != NULL);
	for (std::list<std::list<std::string> >::iterator oepPathIter = this->__idPathList_OtherEntriesPlaceHolders.begin(); oepPathIter != this->__idPathList_OtherEntriesPlaceHolders.end(); oepPathIter++) {
		std::list<Entry>* dataSource = this->dataSource->getListByPath(*oepPathIter);
		if (dataSource) {
			Rule* oep = this->getPlaceholderBySourceList(*dataSource, this->rules);
			assert(oep != NULL);
			Rule* parentRule = this->getParentRule(oep);
			std::list<Rule>& dataTarget = parentRule ? parentRule->subRules : this->rules;

			std::list<Rule>::iterator dataTargetIter = dataTarget.begin();
			while (dataTargetIter != dataTarget.end() && (dataTargetIter->type != Rule::OTHER_ENTRIES_PLACEHOLDER || dataTargetIter->__idpath != *oepPathIter)){
				dataTargetIter++;
			}
			std::list<Rule> newRules;
			for (std::list<Entry>::iterator iter = dataSource->begin(); iter != dataSource->end(); iter++){
				Rule* relatedRule = this->getRuleByEntry(*iter, this->rules);
				if (!relatedRule){
					newRules.push_back(Rule(*iter, dataTargetIter->isVisible, *this->dataSource, this->__idPathList, this->dataSource->buildPath(*iter))); //generate rule for given entry
				}
			}
			dataTargetIter++;
			dataTarget.splice(dataTargetIter, newRules);
		}
	}
}

void Proxy::sync_cleanup(Rule* parent) {
	std::list<Rule>& list = parent ? parent->subRules : this->rules;

	bool done = false;
	do {
		bool listModified = false;
		for (std::list<Rule>::iterator iter = list.begin(); !listModified && iter != list.end(); iter++) {
			if (!(iter->type == Rule::NORMAL && iter->dataSource ||
				  iter->type == Rule::OTHER_ENTRIES_PLACEHOLDER && iter->dataSource_list)) {
				list.erase(iter);
				listModified = true; //after ereasing something we have to create a new iterator
			} else { //check contents
				this->sync_cleanup(&*iter);
			}
		}

		if (!listModified)
			done = true;
	} while (!done);
}

bool Proxy::isModified(Rule const* parent) const {
	bool result = false;
	if (!parent && this->dataSource || parent && parent->dataSource){
		std::list<Rule> const& rlist = parent ? parent->subRules : this->rules;
		std::list<Entry> const& elist = parent ? parent->dataSource->subEntries : *this->dataSource;
		if (rlist.size()-1 == elist.size()){ //rules contains the other entries placeholder, so there is one more entry
			std::list<Rule>::const_iterator ruleIter = rlist.begin();
			std::list<Entry>::const_iterator entryIter = elist.begin();
			if (ruleIter->type == Rule::OTHER_ENTRIES_PLACEHOLDER){ //the first element is the OTHER_ENTRIES_PLACEHOLDER by default.
				result = !ruleIter->isVisible; //If not visible, it's modified…
				ruleIter++;
			}
			while (!result && ruleIter != rlist.end() && entryIter != elist.end()){
				if (ruleIter->outputName != entryIter->name || !ruleIter->isVisible)
					result = true;
				if (ruleIter->dataSource->type == Entry::SUBMENU) {
					result = this->isModified(&*ruleIter);
				}

				ruleIter++;
				entryIter++;
			}
		}
		else
			result = true;
	}
	return result;
}

bool Proxy::deleteFile(){
	int success = unlink(this->fileName.c_str());
	if (success == 0){
		this->fileName = "";
		return true;
	}
	else
		return false;
}

bool Proxy::generateFile(std::string const& path, int cfg_dir_prefix_length, std::string const& cfg_dir_noprefix){
	if (this->dataSource){
		FILE* proxyFile = fopen(path.c_str(), "w");
		if (proxyFile){
			this->fileName = path;
			fputs("#!/bin/sh\n#THIS IS A GRUB PROXY SCRIPT\n", proxyFile);
			fputs(("'"+this->dataSource->fileName.substr(cfg_dir_prefix_length)+"'").c_str(), proxyFile);
			fputs((" | "+cfg_dir_noprefix+"/bin/grubcfg_proxy \"").c_str(), proxyFile);
			for (std::list<Rule>::iterator ruleIter = this->rules.begin(); ruleIter != this->rules.end(); ruleIter++){
				assert(this->dataSource != NULL);
				fputs((ruleIter->toString(*this->dataSource)+"\n").c_str(), proxyFile); //write rule
			}
			fputs("\"", proxyFile);
			fclose(proxyFile);
			chmod(path.c_str(), this->permissions);
			return true;
		}
	}
	return false;
}

bool Proxy::isExecutable() const {
	return this->permissions & 0111;
}


void Proxy::set_isExecutable(bool value){
	if (value)
		permissions |= 0111;
	else
		permissions &= ~0111;
}

std::string Proxy::getScriptName(){
	if (this->dataSource)
		return this->dataSource->name;
	else
		return "?";
}


Rule& Proxy::moveRule(Rule* rule, int direction) {
	std::list<Rule>& ruleList = this->getRuleList(this->getParentRule(rule));

	std::list<Rule>::iterator el = this->getListIterator(*rule, ruleList);
	std::list<Rule>::iterator next = el; adjustIterator(next, direction);

	Rule* newRule = rule;
	if (next == ruleList.end()) { //scale down
		try {
			std::list<Rule>& parentOfParent = this->getRuleList(this->getParentRule(this->getParentRule(rule)));
			std::list<Rule>::iterator parentListIter = this->getListIterator(*this->getParentRule(rule), parentOfParent);
			std::list<Rule>::iterator nextInParent = parentListIter; adjustIterator(nextInParent, direction == 1 ? 1 : 0);

			newRule = &*parentOfParent.insert(nextInParent, *rule);
		} catch (Proxy::Exception e) {
			if (e == RULE_NOT_FOUND)
				throw NO_MOVE_TARGET_FOUND;
			else
				throw e;
		}
	} else if (next->dataSource && next->dataSource->type == Entry::SUBMENU) { //scale up
		if (direction == 1) {
			next->subRules.push_front(*rule);
			newRule = &next->subRules.front();
		} else {
			next->subRules.push_back(*rule);
			newRule = &next->subRules.back();
		}
	} else { //keep on this level
		std::list<Rule>::iterator afterNext = el;adjustIterator(afterNext, direction == 1 ? 2 : -1);
		if (afterNext != ruleList.end()) {
			newRule = &*ruleList.insert(afterNext, *rule);
		} else {
			ruleList.push_back(*rule);
			newRule = &ruleList.back();
		}
	}
	ruleList.erase(this->getListIterator(*rule, ruleList));
	return *newRule;
}

Rule* Proxy::getParentRule(Rule* child, Rule* root) {
	std::list<Rule>& list = root ? root->subRules : this->rules;
	for (std::list<Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (&*iter == child)
			return root;
		else if (iter->subRules.size()) {
			Rule* parentRule = NULL;
			try {
				parentRule = this->getParentRule(child, &*iter);
			} catch (Proxy::Exception e) {
				if (e != RULE_NOT_FOUND)
					throw e;
			}
			if (parentRule) {
				return parentRule;
			}
		}
	}
	throw RULE_NOT_FOUND;
}

std::list<Rule>& Proxy::getRuleList(Rule* parentElement) {
	if (parentElement)
		return parentElement->subRules;
	else
		return this->rules;
}

std::list<Rule>::iterator Proxy::getListIterator(Rule const& needle, std::list<Rule>& haystack) {
	for (std::list<Rule>::iterator iter = haystack.begin(); iter != haystack.end(); iter++) {
		if (&*iter == &needle)
			return iter;
	}

	throw RULE_NOT_FOUND;
}

Rule* Proxy::getPlaceholderBySourceList(std::list<Entry> const& sourceList, std::list<Rule>& baseList) {
	for (std::list<Rule>::iterator iter = baseList.begin(); iter != baseList.end(); iter++) {
		if (iter->dataSource_list == &sourceList) {
			return &*iter;
		} else if (iter->type == Rule::NORMAL && iter->subRules.size()) {
			Rule* result = this->getPlaceholderBySourceList(sourceList, iter->subRules);
			if (result) {
				return result;
			}
		}
	}
	return NULL;
}


void Proxy::adjustIterator(std::list<Rule>::iterator& iter, int adjustment) {
	if (adjustment > 0) {
		for (int i = 0; i < adjustment; i++) {
			iter++;
		}
	} else {
		for (int i = 0; i > adjustment; i--) {
			iter--;
		}
	}
}

