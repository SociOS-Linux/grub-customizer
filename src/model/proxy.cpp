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

Proxy::Proxy(Script& dataSource, bool activateRules)
	: dataSource(&dataSource), permissions(0755), index(90)
{
	rules.push_back(Rule(Rule::OTHER_ENTRIES_PLACEHOLDER, std::list<std::string>(), "*", activateRules));
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
			this->sync_cleanup(NULL, scriptMap);

		return true;
	}
	else
		return false;
}

void Proxy::sync_connectExisting(Rule* parent, std::map<std::string, Script*> scriptMap) {
	assert(this->dataSource != NULL);
	if (parent == NULL) {
		this->__idPathList.clear();
		this->__idPathList_OtherEntriesPlaceHolders.clear();
	}
	std::list<Rule>& list = parent ? parent->subRules : this->rules;
	for (std::list<Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->type != Rule::SUBMENU) { // don't sync submenu entries
			std::list<std::string> path = iter->__idpath;

			Script* script = NULL;
			if (iter->__sourceScriptPath == "") { // main dataSource
				script = this->dataSource;
			} else if (scriptMap.size()) {
				assert(scriptMap.find(iter->__sourceScriptPath) != scriptMap.end()); // expecting that the script exists on the map
				script = scriptMap[iter->__sourceScriptPath];
			} else {
				continue; // don't sync foreign entries if scriptMap is empty
			}

			if (iter->type != Rule::OTHER_ENTRIES_PLACEHOLDER) {
				this->__idPathList[script].push_back(path);
			} else {
				this->__idPathList_OtherEntriesPlaceHolders[script].push_back(path);
			}

			iter->dataSource = script->getEntryByPath(path);

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
			} else if (scriptMap.size()) {
				assert(scriptMap.find(iter->__sourceScriptPath) != scriptMap.end()); // expecting that the script exists on the map
				script = scriptMap[iter->__sourceScriptPath];
			} else {
				continue; // don't sync foreign entries if scriptMap is empty
			}
			iter->dataSource = script->getEntryByHash(iter->__idHash, script->entries());
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
	for (std::map<Script*, std::list<std::list<std::string> > >::iterator scriptIter = this->__idPathList_OtherEntriesPlaceHolders.begin(); scriptIter != this->__idPathList_OtherEntriesPlaceHolders.end(); scriptIter++) {
		for (std::list<std::list<std::string> >::iterator oepPathIter = this->__idPathList_OtherEntriesPlaceHolders[scriptIter->first].begin(); oepPathIter != this->__idPathList_OtherEntriesPlaceHolders[scriptIter->first].end(); oepPathIter++) {
			Entry* dataSource = scriptIter->first->getEntryByPath(*oepPathIter);
			if (dataSource) {
				Rule* oep = this->getRuleByEntry(*dataSource, this->rules, Rule::OTHER_ENTRIES_PLACEHOLDER);
				assert(oep != NULL);
				Rule* parentRule = this->getParentRule(oep);
				std::list<Rule>& dataTarget = parentRule ? parentRule->subRules : this->rules;

				std::list<Rule>::iterator dataTargetIter = dataTarget.begin();
				while (dataTargetIter != dataTarget.end() && !(dataTargetIter->type == Rule::OTHER_ENTRIES_PLACEHOLDER && dataTargetIter->__idpath == *oepPathIter && (dataTargetIter->__sourceScriptPath != "" && scriptMap.size() && scriptMap[dataTargetIter->__sourceScriptPath] == scriptIter->first || dataTargetIter->__sourceScriptPath == "" && scriptIter->first == this->dataSource))){
					dataTargetIter++;
				}
				std::list<Rule> newRules;
				for (std::list<Entry>::iterator iter = dataSource->subEntries.begin(); iter != dataSource->subEntries.end(); iter++){
					Rule* relatedRule = this->getRuleByEntry(*iter, this->rules, Rule::NORMAL);
					Rule* relatedRulePt = this->getRuleByEntry(*iter, this->rules, Rule::PLAINTEXT);
					Rule* relatedRuleOep = this->getRuleByEntry(*iter, this->rules, Rule::OTHER_ENTRIES_PLACEHOLDER);
					if (!relatedRule && !relatedRuleOep && !relatedRulePt){
						newRules.push_back(Rule(*iter, dataTargetIter->isVisible, *scriptIter->first, this->__idPathList[scriptIter->first], scriptIter->first->buildPath(*iter))); //generate rule for given entry
					}
				}
				dataTargetIter++;
				dataTarget.splice(dataTargetIter, newRules);
			}
		}
	}
}

void Proxy::sync_cleanup(Rule* parent, std::map<std::string, Script*> scriptMap) {
	std::list<Rule>& list = parent ? parent->subRules : this->rules;

	bool done = false;
	do {
		bool listModified = false;
		for (std::list<Rule>::iterator iter = list.begin(); !listModified && iter != list.end(); iter++) {
			if (!(iter->type == Rule::NORMAL && iter->dataSource ||
				  iter->type == Rule::SUBMENU && iter->subRules.size() ||
				  iter->type == Rule::OTHER_ENTRIES_PLACEHOLDER && iter->dataSource ||
				  iter->type == Rule::PLAINTEXT && iter->dataSource)) {
				if (iter->__sourceScriptPath == "" || scriptMap.size()) {
					list.erase(iter);
					listModified = true; //after ereasing something we have to create a new iterator
				}
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
	std::list<Entry> const& elist = parentEntry ? parentEntry->subEntries : this->dataSource->entries();
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

std::list<std::string> Proxy::getScriptList(std::map<Entry const*, Script const*> const& entrySourceMap, std::map<Script const*, std::string> const& scriptTargetMap) const {
	std::map<std::string, void*> uniqueList; // the pointer (value) is just a dummy
	for (std::map<Entry const*, Script const*>::const_iterator iter = entrySourceMap.begin(); iter != entrySourceMap.end(); iter++) {
		uniqueList[scriptTargetMap.find(iter->second)->second] = NULL;
	}
	std::list<std::string> result;
	result.push_back(scriptTargetMap.find(this->dataSource)->second); // the own script must be the first entry
	for (std::map<std::string, void*>::iterator iter = uniqueList.begin(); iter != uniqueList.end(); iter++) {
		result.push_back(iter->first);
	}
	return result;
}

bool Proxy::generateFile(std::string const& path, int cfg_dir_prefix_length, std::string const& cfg_dir_noprefix, std::map<Entry const*, Script const*> entrySourceMap, std::map<Script const*, std::string> const& scriptTargetMap){
	if (this->dataSource){
		FILE* proxyFile = fopen(path.c_str(), "w");
		if (proxyFile){
			this->fileName = path;
			fputs("#!/bin/sh\n#THIS IS A GRUB PROXY SCRIPT\n", proxyFile);
			std::list<std::string> scripts = this->getScriptList(entrySourceMap, scriptTargetMap);
			if (scripts.size() == 1) { // single script
				fputs(("'"+this->dataSource->fileName.substr(cfg_dir_prefix_length)+"'").c_str(), proxyFile);
			} else { // multi script
				fputs("sh -c '", proxyFile);
				for (std::list<std::string>::iterator scriptIter = scripts.begin(); scriptIter != scripts.end(); scriptIter++) {
					fputs(("echo \"### BEGIN "+(*scriptIter).substr(cfg_dir_prefix_length)+" ###\";\n").c_str(), proxyFile);
					fputs(("\""+(*scriptIter).substr(cfg_dir_prefix_length)+"\";\n").c_str(), proxyFile);
					fputs(("echo \"### END "+(*scriptIter).substr(cfg_dir_prefix_length)+" ###\";").c_str(), proxyFile);
					if (&*scriptIter != &scripts.back()) {
						fputs("\n", proxyFile);
					}
				}
				fputs("'", proxyFile);
			}
			fputs((" | "+cfg_dir_noprefix+"/bin/grubcfg_proxy \"").c_str(), proxyFile);
			for (std::list<Rule>::iterator ruleIter = this->rules.begin(); ruleIter != this->rules.end(); ruleIter++){
				assert(this->dataSource != NULL);
				EntryPathBuilderImpl entryPathBuilder(*this->dataSource);
				entryPathBuilder.setScriptTargetMap(scriptTargetMap);
				entryPathBuilder.setEntrySourceMap(entrySourceMap);
				fputs((ruleIter->toString(entryPathBuilder)+"\n").c_str(), proxyFile); //write rule
			}
			fputs("\"", proxyFile);
			if (scripts.size() > 1) {
				fputs(" multi", proxyFile);
			}
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
	std::list<Rule>::iterator next = this->getNextVisibleRule(el, direction);

	Rule* parent = NULL;
	try {
		parent = this->getParentRule(&*next);
	} catch (Proxy::Exception e) {} // leave parent in NULL state
	std::list<Rule>& list = this->getRuleList(parent);

	Rule* newRule = rule;

	std::list<Rule>::iterator afterNext = next;

	if (direction == 1 && &*next != &list.front()) {
		afterNext++;
	} else if (direction == -1 && &*next == &list.back()) {
		afterNext++;
	}

	newRule = &*list.insert(afterNext, *rule);

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

std::list<Rule>::iterator Proxy::getNextVisibleRule(std::list<Rule>::iterator base, int direction) {
	assert(direction == -1 || direction == 1);
	Rule* parent = NULL;
	try {
		parent = this->getParentRule(&*base);
	} catch (Proxy::Exception e) {} // leave parent in NULL state
	std::list<Rule>* list = &this->getRuleList(parent);

	do {
		if (direction == 1) {
			base++;
		} else {
			base--;
		}
		if (base == list->end()) { // go one step up
			if (parent == NULL) { // already top level
				throw Proxy::NO_MOVE_TARGET_FOUND;
			}
			Rule* currentElement = parent;
			try {
				parent = this->getParentRule(currentElement); //may throw Proxy::RULE_NOT_FOUND
			} catch (Proxy::Exception e) {} // leave parent in NULL state
			list = &this->getRuleList(parent);
			base = this->getListIterator(*currentElement, *list);
		} else if (base->type == Rule::SUBMENU) {
			parent = &*base;
			list = &parent->subRules;
			if (direction == -1) {
				base = list->end();
				base--;
			} else {
				base = list->begin();
			}
		}
	} while (!base->isVisible && base != list->end());
	return base;
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

bool Proxy::ruleIsFromOwnScript(Rule const& rule) const {
	assert(this->dataSource != NULL);
	assert(rule.dataSource != NULL);
	if (this->dataSource->hasEntry(*rule.dataSource)) {
		return true;
	} else {
		return false;
	}
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

std::list<Rule>::iterator Proxy::getListIterator(Rule const& needle, std::list<Rule>& haystack) {
	for (std::list<Rule>::iterator iter = haystack.begin(); iter != haystack.end(); iter++) {
		if (&*iter == &needle)
			return iter;
	}

	throw RULE_NOT_FOUND;
}

std::list<Rule>& Proxy::getRuleList(Rule* parentElement) {
	if (parentElement)
		return parentElement->subRules;
	else
		return this->rules;
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

void Proxy::removeForeignChildRules(Rule& parent) {
	bool loopRestartRequired = false;
	do { // required to restart the loop after an entry has been removed
		loopRestartRequired = false;
		for (std::list<Rule>::iterator iter = parent.subRules.begin(); iter != parent.subRules.end(); iter++) {
			if (iter->dataSource) {
				if (!this->ruleIsFromOwnScript(*iter)) {
					parent.subRules.erase(iter);
					loopRestartRequired = true;
					break;
				}
			} else if (iter->subRules.size()) {
				this->removeForeignChildRules(*iter);
				if (iter->subRules.size() == 0) { // if this submenu is empty now, remove it
					parent.subRules.erase(iter);
					loopRestartRequired = true;
					break;
				}
			}
		}
	} while (loopRestartRequired);
}

void Proxy::removeEquivalentRules(Rule const& base) {
	if (base.dataSource) {
		Rule* eqRule = this->getRuleByEntry(*base.dataSource, this->rules, base.type);
		if (eqRule) {
			Rule* parent = NULL;
			int rlist_size = 0;
			do {
				try {
					parent = this->getParentRule(eqRule);
				} catch (Proxy::Exception e) {
					if (e == Proxy::RULE_NOT_FOUND) {
						parent = NULL;
					} else {
						throw e;
					}
				}
				std::list<Rule>& rlist = this->getRuleList(parent);
				std::list<Rule>::iterator iter = this->getListIterator(*eqRule, rlist);
				rlist.erase(iter);

				eqRule = parent; // go one step up to remove this rule if empty
				rlist_size = rlist.size();
			} while (rlist_size == 0 && parent != NULL); // delete all the empty submenus above
		}
	} else if (base.subRules.size()) {
		for (std::list<Rule>::const_iterator iter = base.subRules.begin(); iter != base.subRules.end(); iter++) {
			this->removeEquivalentRules(*iter);
		}
	}
}



