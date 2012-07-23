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

	bool inString = false, inAlias = false, inHash = false, inFromClause = false;
	std::string name;
	std::string hash;
	std::list<std::string> path;
	bool visible = false;
	const char* iter = NULL;
	for (iter = *ruleString; *iter && (*iter != '}' || inString || inAlias || inFromClause); iter++) {
		if (!inString && *iter == '+') {
			visible = true;
		} else if (!inString && *iter == '-') {
			visible = false;
		} else if (*iter == '\'' && iter[1] != '\'') {
			inString = !inString;
			if (iter[1] != '/') {
				if (!inString){
					if (inAlias) {
						rules.back().outputName = name;
					} else if (inFromClause) {
						rules.back().__sourceScriptPath = name;
					} else {
						path.push_back(name);
						rules.push_back(Rule(Rule::NORMAL, path, visible));
						path.clear();
					}
					inAlias = false;
					inFromClause = false;
				}
				name = "";
			}
		} else if (!inString && *iter == '*') {
			rules.push_back(Rule(Rule::OTHER_ENTRIES_PLACEHOLDER, path, "*", visible));
			path.clear();
		} else if (!inString && *iter == '#' && *++iter == 't' && *++iter == 'e' && *++iter == 'x' && *++iter == 't') {
			path.push_back("#text");
			rules.push_back(Rule(Rule::PLAINTEXT, path, "#text", visible));
			path.clear();
			name = "";
		} else if (inString) {
			name += *iter;
			if (*iter == '\'')
				iter++;
		} else if (inHash && *iter != '~') {
			hash += *iter;
		} else if (!inString && !inAlias && !inFromClause && *iter == 'a' && *++iter == 's') {
			inAlias = true;
		} else if (!inString && !inAlias && !inFromClause && *iter == 'f' && *++iter == 'r' && *++iter == 'o' && *++iter == 'm') {
			inFromClause = true;
		} else if (!inString && !inAlias && !inFromClause && *iter == '/') {
			path.push_back(name);
			name = "";
		} else if (!inString && !inAlias && !inFromClause && *iter == '{') {
			iter++;
			rules.back().subRules = Proxy::parseRuleString(&iter);
			rules.back().type = Rule::SUBMENU;
		} else if (!inString && *iter == '~') {
			inHash = !inHash;
			if (!inHash) {
				rules.back().__idHash = hash;
				hash = "";
			}
		}
	}
	*ruleString = iter;
	return rules;
}

void Proxy::importRuleString(const char* ruleString){
	rules = Proxy::parseRuleString(&ruleString);
}

Rule* Proxy::getRuleByEntry(Entry const& entry, std::list<Rule>& list, Rule::RuleType ruletype) {
	for (std::list<Rule>::iterator rule_iter = list.begin(); rule_iter != list.end(); rule_iter++){
		if (&entry == rule_iter->dataSource && rule_iter->type == ruletype)
			return &*rule_iter;
		else {
			Rule* result = this->getRuleByEntry(entry, rule_iter->subRules, ruletype);
			if (result)
				return result;
		}
	}
	return NULL;
}

void Proxy::unsync(Rule* parent) {
	std::list<Rule>& list = parent ? parent->subRules : this->rules;
	for (std::list<Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		iter->dataSource = NULL;
		iter->dataSource_list = NULL;
		if (iter->subRules.size()) {
			this->unsync(&*iter);
		}
	}
}

bool Proxy::sync(bool deleteInvalidRules, bool expand, std::map<std::string, Script*> scriptMap){
	if (this->dataSource){
		this->sync_connectExisting(NULL, scriptMap);
		this->sync_connectExistingByHash(NULL, scriptMap);
		if (expand) {
			this->sync_add_placeholders(NULL, scriptMap);
			this->sync_expand(scriptMap);
		}

		if (deleteInvalidRules)
			this->sync_cleanup();

		return true;
	}
	else
		return false;
}

void Proxy::sync_connectExisting(Rule* parent, std::map<std::string, Script*> scriptMap) {
	assert(this->dataSource != NULL);
	if (parent == NULL) {
		this->__idPathList.clear();
	}
	std::list<Rule>& list = parent ? parent->subRules : this->rules;
	for (std::list<Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->type != Rule::SUBMENU) { // don't sync submenu entries
			std::list<std::string> path = iter->__idpath;

			Script* script = NULL;
			if (iter->__sourceScriptPath == "") { // main dataSource
				script = this->dataSource;
			} else {
				assert(scriptMap.find(iter->__sourceScriptPath) != scriptMap.end()); // expecting that the script exists on the map
				script = scriptMap[iter->__sourceScriptPath];
			}

			if (iter->type != Rule::OTHER_ENTRIES_PLACEHOLDER) {
				this->__idPathList[script].push_back(path);
			} else {
				this->__idPathList_OtherEntriesPlaceHolders[script].push_back(path);
			}

			iter->dataSource = script->getEntryByPath(path);

			if (iter->type == Rule::OTHER_ENTRIES_PLACEHOLDER) {
				iter->dataSource_list = script->getListByPath(path);
			}
		} else if (iter->subRules.size()) {
			this->sync_connectExisting(&*iter, scriptMap);
		}
	}
}

void Proxy::sync_connectExistingByHash(Rule* parent, std::map<std::string, Script*> scriptMap) {
	assert(this->dataSource != NULL);
	std::list<Rule>& list = parent ? parent->subRules : this->rules;
	for (std::list<Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->dataSource == NULL && iter->__idHash != "") {
			Script* script = NULL;
			if (iter->__sourceScriptPath == "") {
				script = this->dataSource;
			} else {
				assert(scriptMap.find(iter->__sourceScriptPath) != scriptMap.end()); // expecting that the script exists on the map
				script = scriptMap[iter->__sourceScriptPath];
			}
			iter->dataSource = script->getEntryByHash(iter->__idHash, *script);
			if (iter->dataSource) {
				this->__idPathList[script].push_back(script->buildPath(*iter->dataSource));
			}
		}
		if (iter->subRules.size()) {
			this->sync_connectExistingByHash(&*iter, scriptMap);
		}
	}
}

void Proxy::sync_add_placeholders(Rule* parent, std::map<std::string, Script*> scriptMap) {
	assert(parent == NULL || parent->dataSource != NULL);

	std::list<std::string> path = parent ? this->dataSource->buildPath(*parent->dataSource) : std::list<std::string>();
	//find out if currentPath is on the blacklist
	bool eop_is_blacklisted = false;

	for (std::list<std::list<std::string> >::const_iterator pti_iter = this->__idPathList_OtherEntriesPlaceHolders[this->dataSource].begin(); pti_iter != this->__idPathList_OtherEntriesPlaceHolders[this->dataSource].end(); pti_iter++) {
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
		this->__idPathList_OtherEntriesPlaceHolders[this->dataSource].push_back(path);
	}

	//sub entries (recursion)
	for (std::list<Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->dataSource && iter->type == Rule::SUBMENU) {
			this->sync_add_placeholders(&*iter);
		}
	}
}


void Proxy::sync_expand(std::map<std::string, Script*> scriptMap) {
	assert(this->dataSource != NULL);
	for (std::map<std::string, Script*>::iterator scriptIter = scriptMap.begin(); scriptIter != scriptMap.end(); scriptIter++) {
		for (std::list<std::list<std::string> >::iterator oepPathIter = this->__idPathList_OtherEntriesPlaceHolders[scriptIter->second].begin(); oepPathIter != this->__idPathList_OtherEntriesPlaceHolders[scriptIter->second].end(); oepPathIter++) {
			std::list<Entry>* dataSource = scriptIter->second->getListByPath(*oepPathIter);
			if (dataSource) {
				Rule* oep = this->getPlaceholderBySourceList(*dataSource, this->rules);
				assert(oep != NULL);
				Rule* parentRule = this->getParentRule(oep);
				std::list<Rule>& dataTarget = parentRule ? parentRule->subRules : this->rules;

				std::list<Rule>::iterator dataTargetIter = dataTarget.begin();
				while (dataTargetIter != dataTarget.end() && (dataTargetIter->type != Rule::OTHER_ENTRIES_PLACEHOLDER || dataTargetIter->__idpath != *oepPathIter || dataTargetIter->__sourceScriptPath != scriptIter->first)){
					dataTargetIter++;
				}
				std::list<Rule> newRules;
				for (std::list<Entry>::iterator iter = dataSource->begin(); iter != dataSource->end(); iter++){
					Rule* relatedRule = this->getRuleByEntry(*iter, this->rules, Rule::NORMAL);
					Rule* relatedRulePt = this->getRuleByEntry(*iter, this->rules, Rule::PLAINTEXT);
					Rule* relatedRuleOep = this->getRuleByEntry(*iter, this->rules, Rule::OTHER_ENTRIES_PLACEHOLDER);
					if (!relatedRule && !relatedRuleOep && !relatedRulePt){
						newRules.push_back(Rule(*iter, dataTargetIter->isVisible, *scriptIter->second, this->__idPathList[scriptIter->second], scriptIter->second->buildPath(*iter))); //generate rule for given entry
					}
				}
				dataTargetIter++;
				dataTarget.splice(dataTargetIter, newRules);
			}
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
				  iter->type == Rule::SUBMENU && iter->subRules.size() ||
				  iter->type == Rule::OTHER_ENTRIES_PLACEHOLDER && iter->dataSource_list ||
				  iter->type == Rule::PLAINTEXT && iter->dataSource)) {
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

bool Proxy::isModified(Rule const* parentRule, Entry const* parentEntry) const {
	assert(this->dataSource != NULL);
	bool result = false;

	std::list<Rule> const& rlist = parentRule ? parentRule->subRules : this->rules;
	std::list<Entry> const& elist = parentEntry ? parentEntry->subEntries : *this->dataSource;
	if (rlist.size()-1 == elist.size()){ //rules contains the other entries placeholder, so there is one more entry
		std::list<Rule>::const_iterator ruleIter = rlist.begin();
		std::list<Entry>::const_iterator entryIter = elist.begin();
		if (ruleIter->type == Rule::OTHER_ENTRIES_PLACEHOLDER){ //the first element is the OTHER_ENTRIES_PLACEHOLDER by default.
			result = !ruleIter->isVisible; //If not visible, it's modified…
			ruleIter++;
		}
		while (!result && ruleIter != rlist.end() && entryIter != elist.end()){
			if (ruleIter->outputName != entryIter->name || !ruleIter->isVisible) {
				result = true;
			} else if (ruleIter->type == Rule::SUBMENU) {
				result = this->isModified(&*ruleIter, &*entryIter);
			}

			ruleIter++;
			entryIter++;
		}
	} else {
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
	} else if (next->type == Rule::SUBMENU) { //scale up
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

	if (ruleList.size() == 1) {
		// delete parent list if empty
		Rule* parent = this->getParentRule(rule);
		std::list<Rule>& parentOfParent = this->getRuleList(this->getParentRule(this->getParentRule(rule)));
		parentOfParent.erase(this->getListIterator(*parent, parentOfParent));
	} else {
		ruleList.erase(this->getListIterator(*rule, ruleList));
	}


	return *newRule;
}

Rule* Proxy::removeSubmenu(Rule* childItem) {
	Rule* parent = this->getParentRule(childItem);

	// search items before and after the submenu
	std::list<Rule> rulesBefore;
	std::list<Rule> rulesAfter;
	bool isBehindChildtem = false;
	for (std::list<Rule>::iterator iter = parent->subRules.begin(); iter != parent->subRules.end(); iter++) {
		if (&*iter != childItem) {
			if (!isBehindChildtem) {
				rulesBefore.push_back(*iter);
			} else {
				rulesAfter.push_back(*iter);
			}
		} else {
			isBehindChildtem = true;
		}
	}
	Rule oldSubmenu = *parent;
	oldSubmenu.subRules.clear();

	// replace the submenu
	*parent = *childItem;

	// add the rules before and/or after to new submenus
	if (rulesBefore.size() || rulesAfter.size()) {
		std::list<Rule>* list = NULL;
		Rule* parentRule = this->getParentRule(parent);
		if (parentRule) {
			list = &parentRule->subRules;
		} else {
			list = &this->rules;
		}
		assert(list != NULL);

		if (rulesBefore.size()) {
			Rule newSubmenu = oldSubmenu;
			newSubmenu.subRules = rulesBefore;
			list->insert(this->getListIterator(*parent, *list), newSubmenu);
		}

		if (rulesAfter.size()) {
			Rule newSubmenu = oldSubmenu;
			newSubmenu.subRules = rulesAfter;
			std::list<Rule>::iterator iter = this->getListIterator(*parent, *list);
			iter++;
			list->insert(iter, newSubmenu);
		}
	}

	return parent;
}

Rule* Proxy::createSubmenu(Rule* childItem) {
	Rule childCopy = *childItem;
	*childItem = Rule(Rule::SUBMENU, std::list<std::string>(), "", true);
	childItem->subRules.push_back(childCopy);
	return childItem;
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
		} else if (iter->type == Rule::SUBMENU && iter->subRules.size()) {
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

