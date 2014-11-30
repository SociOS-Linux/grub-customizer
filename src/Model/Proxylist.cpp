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

#ifndef GRUB_CUSTOMIZER_PROXYLIST_INCLUDED
#define GRUB_CUSTOMIZER_PROXYLIST_INCLUDED
//LZZ: #hdr
#include <list>
#include <sstream>
#include "Proxy.cpp"
#include "../lib/Trait/LoggerAware.h"
#include "../lib/Exception.cpp"
#include "../lib/ArrayStructure.cpp"
//LZZ: #end

struct Model_Proxylist_Item {
	std::string labelPathValue;
	std::string labelPathLabel;
	std::string numericPathValue;
	std::string numericPathLabel;
};
struct Model_Proxylist : public std::list<Model_Proxy>, public Trait_LoggerAware {
	std::list<Model_Proxy> trash; //removed proxies
	std::list<Model_Proxy*> getProxiesByScript(Model_Script const& script) {
		std::list<Model_Proxy*> result;
		for (Model_Proxylist::iterator iter = this->begin(); iter != this->end(); iter++){
			if (iter->dataSource == &script)
				result.push_back(&*iter);
		}
		return result;
	}

	std::list<const Model_Proxy*> getProxiesByScript(Model_Script const& script) const {
		std::list<const Model_Proxy*> result;
		for (Model_Proxylist::const_iterator iter = this->begin(); iter != this->end(); iter++){
			if (iter->dataSource == &script)
				result.push_back(&*iter);
		}
		return result;
	}

	std::list<Model_Rule*> getForeignRules() {
		std::list<Model_Rule*> result;
	
		for (std::list<Model_Proxy>::iterator proxyIter = this->begin(); proxyIter != this->end(); proxyIter++) {
			std::list<Model_Rule*> subResult = proxyIter->getForeignRules();
			result.splice(result.end(), subResult);
		}
	
		return result;
	}

	void sync_all(bool deleteInvalidRules = true, bool expand = true, Model_Script* relatedScript = NULL, std::map<std::string, Model_Script*> scriptMap = std::map<std::string, Model_Script*>() ) { //relatedScript = NULL: sync all proxies, otherwise only sync proxies wich target the given Script
		for (Model_Proxylist::iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
			if (relatedScript == NULL || proxy_iter->dataSource == relatedScript)
				proxy_iter->sync(deleteInvalidRules, expand, scriptMap);
		}	
	}
 //relatedScript = NULL: sync all proxies, otherwise only sync proxies wich target the given Script
	void unsync_all() {
		for (Model_Proxylist::iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
			proxy_iter->unsync();
		}
	}

	bool proxyRequired(Model_Script const& script) const {
		std::list<const Model_Proxy*> plist = this->getProxiesByScript(script);
		if (plist.size() == 1){
			return plist.front()->isModified();
		}
		else
			return true;
	}

	void deleteAllProxyscriptFiles() {
		for (Model_Proxylist::iterator iter = this->begin(); iter != this->end(); iter++){
			if (iter->fileName != "" && iter->dataSource && iter->dataSource->fileName != iter->fileName){
				iter->deleteFile();
			}
		}
	}

	static bool compare_proxies(Model_Proxy const& a, Model_Proxy const& b) {
		if (a.index != b.index) {
			return a.index < b.index;
		} else {
			if (a.dataSource != NULL && b.dataSource != NULL) {
				return a.dataSource->name < b.dataSource->name;
			} else {
				return true;
			}
		}
	}

	void sort() {
		std::list<Model_Proxy>::sort(Model_Proxylist::compare_proxies);
	}

	void deleteProxy(Model_Proxy* proxyPointer) {
		for (Model_Proxylist::iterator iter = this->begin(); iter != this->end(); iter++){
			if (&*iter == proxyPointer){
				//if the file must be deleted when saving, move it to trash
				if (proxyPointer->fileName != "" && proxyPointer->dataSource && proxyPointer->fileName != proxyPointer->dataSource->fileName)
					this->trash.push_back(*proxyPointer);
				//remove the proxy object
				this->erase(iter);
				break;
			}
		}
	}

	void clearTrash() {
		for (std::list<Model_Proxy>::iterator iter = this->trash.begin(); iter != this->trash.end(); iter++){
			if (iter->fileName != "") {
				iter->deleteFile();
			}
		}
	}

	std::list<Model_Proxylist_Item> generateEntryTitleList() const {
		std::list<Model_Proxylist_Item> result;
		int offset = 0;
		for (Model_Proxylist::const_iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
			if (proxy_iter->isExecutable()){
				std::list<Model_Proxylist_Item> subList = Model_Proxylist::generateEntryTitleList(proxy_iter->rules, "", "", "", &offset);
				result.splice(result.end(), subList);
			}
		}
		return result;
	}

	std::list<std::string> getToplevelEntryTitles() const {
		std::list<std::string> result;
		for (Model_Proxylist::const_iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
			if (proxy_iter->isExecutable()){
				for (std::list<Model_Rule>::const_iterator rule_iter = proxy_iter->rules.begin(); rule_iter != proxy_iter->rules.end(); rule_iter++) {
					if (rule_iter->isVisible && rule_iter->type == Model_Rule::NORMAL) {
						result.push_back(rule_iter->outputName);
					}
				}
			}
		}
		return result;
	}

	static std::list<Model_Proxylist_Item> generateEntryTitleList(std::list<Model_Rule> const& parent, std::string const& labelPathPrefix, std::string const& numericPathPrefix, std::string const& numericPathLabelPrefix, int* offset = NULL) {
		std::list<Model_Proxylist_Item> result;
		int i = (offset != NULL ? *offset : 0);
		for (std::list<Model_Rule>::const_iterator rule_iter = parent.begin(); rule_iter != parent.end(); rule_iter++){
			if (rule_iter->isVisible && (rule_iter->type == Model_Rule::NORMAL || rule_iter->type == Model_Rule::SUBMENU)) {
				std::ostringstream currentNumPath;
				currentNumPath << numericPathPrefix << i;
				std::ostringstream currentLabelNumPath;
				currentLabelNumPath << numericPathLabelPrefix << (i+1);

				bool addedSomething = true;
				if (rule_iter->type == Model_Rule::SUBMENU) {
					std::list<Model_Proxylist_Item> subList = Model_Proxylist::generateEntryTitleList(rule_iter->subRules, labelPathPrefix + rule_iter->outputName + ">", currentNumPath.str() + ">", currentLabelNumPath.str() + ">");
					if (subList.size() == 0) {
						addedSomething = false;
					}
					result.splice(result.end(), subList);
				} else {
					Model_Proxylist_Item newItem;
					newItem.labelPathLabel = rule_iter->outputName;
					newItem.labelPathValue = labelPathPrefix + rule_iter->outputName;
					newItem.numericPathLabel = currentLabelNumPath.str();
					newItem.numericPathValue = currentNumPath.str();
					result.push_back(newItem);
				}
				if (addedSomething) {
					i++;
				}
			}
		}
		if (offset != NULL) {
			*offset = i;
		}
		return result;
	}

	Model_Proxy* getProxyByRule(Model_Rule* rule, std::list<Model_Rule> const& list, Model_Proxy& parentProxy) {
		for (std::list<Model_Rule>::const_iterator rule_iter = list.begin(); rule_iter != list.end(); rule_iter++){
			if (&*rule_iter == rule)
				return &parentProxy;
			else {
				try {
					return this->getProxyByRule(rule, rule_iter->subRules, parentProxy);
				} catch (ItemNotFoundException const& e) {
					// do nothing
				}
			}
		}
		throw ItemNotFoundException("proxy by rule not found", __FILE__, __LINE__);
	}

	Model_Proxy* getProxyByRule(Model_Rule* rule) {
		for (Model_Proxylist::iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
			try {
				return this->getProxyByRule(rule, proxy_iter->rules, *proxy_iter);
			} catch (ItemNotFoundException const& e) {
				// do nothing
			}
		}
		throw ItemNotFoundException("proxy by rule not found", __FILE__, __LINE__);
	}

	std::list<Model_Rule>::iterator moveRuleToNewProxy(Model_Rule& rule, int direction, Model_Script* dataSource = NULL) {
		Model_Proxy* currentProxy = this->getProxyByRule(&rule);
		std::list<Model_Proxy>::iterator proxyIter = this->begin();
		for (;proxyIter != this->end() && &*proxyIter != currentProxy; proxyIter++) {}
	
		if (direction == 1) {
			proxyIter++;
		}
		if (dataSource == NULL) {
			dataSource = currentProxy->dataSource;
		}
		std::list<Model_Proxy>::iterator newProxy = this->insert(proxyIter, Model_Proxy(*dataSource, false));
		newProxy->removeEquivalentRules(rule);
		std::list<Model_Rule>::iterator movedRule = newProxy->rules.insert(direction == -1 ? newProxy->rules.end() : newProxy->rules.begin(), rule);
		rule.setVisibility(false);
	
		if (!currentProxy->hasVisibleRules()) {
			this->deleteProxy(currentProxy);
		}
		return movedRule;
	}

	std::list<Model_Rule>::iterator getNextVisibleRule(Model_Rule* base, int direction) {
		Model_Proxy* proxy = this->getProxyByRule(base);
		std::list<Model_Rule>::iterator iter = proxy->getListIterator(*base, proxy->getRuleList(proxy->getParentRule(base, NULL)));
		return this->getNextVisibleRule(iter, direction);
	}

	std::list<Model_Rule>::iterator getNextVisibleRule(std::list<Model_Rule>::iterator base, int direction) {
		std::list<Model_Proxy>::iterator proxyIter = this->begin();
		{
			Model_Proxy* proxy = this->getProxyByRule(&*base);
			for (;proxyIter != this->end() && &*proxyIter != proxy; proxyIter++) {}
		}

		bool hasParent = false;
		if (proxyIter->getParentRule(&*base)) {
			hasParent = true;
		}

		while (proxyIter != this->end()) {
			try {
				return proxyIter->getNextVisibleRule(base, direction);
			} catch (NoMoveTargetException const& e) {

				if (hasParent) {
					throw NoMoveTargetException("next visible rule not found", __FILE__, __LINE__);
				}

				if (direction == 1) {
					proxyIter++;
					if (proxyIter == this->end()) {
						throw NoMoveTargetException("next visible rule not found", __FILE__, __LINE__);
					}
					base = proxyIter->rules.begin();
				} else {
					proxyIter--;
					if (proxyIter == this->end()) {
						throw NoMoveTargetException("next visible rule not found", __FILE__, __LINE__);
					}
					base = proxyIter->rules.end();
					base--;
				}
				if (base->isVisible) {
					return base;
				}
			}
		}
		throw NoMoveTargetException("next visible rule not found", __FILE__, __LINE__);
	}

	std::list<Model_Proxy>::iterator getIter(Model_Proxy const* proxy) {
		std::list<Model_Proxy>::iterator iter = this->begin();
		while (iter != this->end()) {
			if (&*iter == proxy) {
				break;
			}
			iter++;
		}
		return iter;
	}

	void splitProxy(Model_Proxy const* proxyToSplit, Model_Rule const* firstRuleOfPart2, int direction) {
		std::list<Model_Proxy>::iterator iter = this->getIter(proxyToSplit);
		Model_Proxy* sourceProxy = &*iter;
		if (direction == 1) {
			iter++;
		}
		Model_Proxy* newProxy = &*this->insert(iter, Model_Proxy(*sourceProxy->dataSource, false));
	
		bool isSecondPart = false;
		if (direction == 1) {
			for (std::list<Model_Rule>::iterator ruleIter = sourceProxy->rules.begin(); ruleIter != sourceProxy->rules.end(); ruleIter++) {
				if (&*ruleIter == firstRuleOfPart2) {
					isSecondPart = true;
				}
				if (isSecondPart) {
					newProxy->removeEquivalentRules(*ruleIter);
					newProxy->rules.push_back(*ruleIter);
					ruleIter->isVisible = false;
				}
			}
		} else {
			for (std::list<Model_Rule>::reverse_iterator ruleIter = sourceProxy->rules.rbegin(); ruleIter != sourceProxy->rules.rend(); ruleIter++) {
				if (&*ruleIter == firstRuleOfPart2) {
					isSecondPart = true;
				}
				if (isSecondPart) {
					newProxy->removeEquivalentRules(*ruleIter);
					newProxy->rules.push_front(*ruleIter);
					ruleIter->isVisible = false;
				}
			}
		}
	}

	Model_Rule* getVisibleRuleForEntry(Model_Entry const& entry) {
		for (std::list<Model_Proxy>::iterator proxyIter = this->begin(); proxyIter != this->end(); proxyIter++) {
			if (proxyIter->isExecutable()) {
				Model_Rule* result = proxyIter->getVisibleRuleForEntry(entry);
				if (result) {
					return result;
				}
			}
		}
		return NULL;
	}

	bool hasConflicts() const {
		std::map<std::string, bool> resources; // key: combination of number, "_" and name. Value: true if used before
		for (std::list<Model_Proxy>::const_iterator proxyIter = this->begin(); proxyIter != this->end(); proxyIter++) {
			assert(proxyIter->dataSource); // assume all proxies are having a datasource
			std::ostringstream resourceName;
			resourceName << proxyIter->index << "_" << proxyIter->dataSource->name;
			if (resources[resourceName.str()]) {
				return true;
			} else {
				resources[resourceName.str()] = true;
			}
		}
		return false;
	}

	bool hasProxy(Model_Proxy* proxy) {
		for (std::list<Model_Proxy>::iterator proxyIter = this->begin(); proxyIter != this->end(); proxyIter++) {
			if (&*proxyIter == proxy) {
				return true;
			}
		}
		return false;
	}

	operator ArrayStructure() const {
		ArrayStructure result;
		int trashIterPos = 0;
		result["trash"].isArray = true;
		for (std::list<Model_Proxy>::const_iterator trashIter = this->trash.begin(); trashIter != this->trash.end(); trashIter++) {
			result["trash"][trashIterPos] = ArrayStructure(*trashIter);
			trashIterPos++;
		}
		int itemsIterPos = 0;
		result["(items)"].isArray = true;
		for (std::list<Model_Proxy>::const_iterator itemIter = this->begin(); itemIter != this->end(); itemIter++) {
			result["(items)"][itemsIterPos] = ArrayStructure(*itemIter);
			itemsIterPos++;
		}
		return result;
	}

};

#endif
