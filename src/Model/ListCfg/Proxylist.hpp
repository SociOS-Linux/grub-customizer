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
#include <list>
#include <sstream>
#include <memory>
#include "../Logger/Trait/LoggerAware.hpp"
#include "../../Common/Exception.hpp"
#include "../../Common/ArrayStructure/Container.hpp"
#include "Proxy.hpp"
#include "ProxylistItem.hpp"


namespace Gc { namespace Model { namespace ListCfg { class Proxylist :
	public std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>>,
	public Gc::Model::Logger::Trait::LoggerAware
{
	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>> trash; //removed proxies

	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>> getProxiesByScript(std::shared_ptr<Gc::Model::ListCfg::Script> script)
	{
		std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>> result;
		for (auto proxy : *this) {
			if (proxy->dataSource == script)
				result.push_back(proxy);
		}
		return result;
	}

	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>> getProxiesByScript(std::shared_ptr<Gc::Model::ListCfg::Script> script) const
	{
		std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>> result;
		for (auto proxy : *this){
			if (proxy->dataSource == script)
				result.push_back(proxy);
		}
		return result;
	}

	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>> getForeignRules()
	{
		std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>> result;
	
		for (auto proxy : *this) {
			auto subResult = proxy->getForeignRules();
			result.splice(result.end(), subResult);
		}
	
		return result;
	}

	//relatedScript = NULL: sync all proxies, otherwise only sync proxies wich target the given Script
	public: void sync_all(
		bool deleteInvalidRules = true,
		bool expand = true,
		std::shared_ptr<Gc::Model::ListCfg::Script> relatedScript = nullptr,
		std::map<std::string, std::shared_ptr<Gc::Model::ListCfg::Script>> scriptMap = std::map<std::string, std::shared_ptr<Gc::Model::ListCfg::Script>>()
	) {
		for (auto proxy : *this) {
			if (relatedScript == nullptr || proxy->dataSource == relatedScript)
				proxy->sync(deleteInvalidRules, expand, scriptMap);
		}	
	}

	public: void unsync_all()
	{
		for (auto proxy : *this) {
			proxy->unsync();
		}
	}

	public: bool proxyRequired(std::shared_ptr<Gc::Model::ListCfg::Script> script) const
	{
		auto plist = this->getProxiesByScript(script);
		if (plist.size() == 1){
			return plist.front()->isModified();
		}
		else
			return true;
	}

	public: void deleteAllProxyscriptFiles()
	{
		for (auto proxy : *this) {
			if (proxy->fileName != "" && proxy->dataSource && proxy->dataSource->fileName != proxy->fileName){
				proxy->deleteFile();
			}
		}
	}

	public: static bool compare_proxies(std::shared_ptr<Gc::Model::ListCfg::Proxy> const& a, std::shared_ptr<Gc::Model::ListCfg::Proxy> const& b)
	{
		if (a->index != b->index) {
			return a->index < b->index;
		} else {
			if (a->dataSource != nullptr && b->dataSource != nullptr) {
				return a->dataSource->name < b->dataSource->name;
			} else {
				return true;
			}
		}
	}

	public: void sort()
	{
		std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>>::sort(Gc::Model::ListCfg::Proxylist::compare_proxies);
	}

	public: void deleteProxy(std::shared_ptr<Gc::Model::ListCfg::Proxy> proxyPointer) {
		for (auto proxyIter = this->begin(); proxyIter != this->end(); proxyIter++) {
			if (*proxyIter == proxyPointer){
				//if the file must be deleted when saving, move it to trash
				if (proxyPointer->fileName != "" && proxyPointer->dataSource && proxyPointer->fileName != proxyPointer->dataSource->fileName)
					this->trash.push_back(proxyPointer);
				//remove the proxy object
				this->erase(proxyIter);
				break;
			}
		}
	}

	public: void clearTrash()
	{
		for (auto trashedProxy : this->trash){
			if (trashedProxy->fileName != "") {
				trashedProxy->deleteFile();
			}
		}
	}

	public: std::list<Gc::Model::ListCfg::ProxylistItem> generateEntryTitleList() const
	{
		std::list<Gc::Model::ListCfg::ProxylistItem> result;
		int offset = 0;
		for (auto proxy : *this) {
			if (proxy->isExecutable()){
				std::list<Gc::Model::ListCfg::ProxylistItem> subList = Gc::Model::ListCfg::Proxylist::generateEntryTitleList(proxy->rules, "", "", "", &offset);
				result.splice(result.end(), subList);
			}
		}
		return result;
	}

	public: std::list<std::string> getToplevelEntryTitles() const
	{
		std::list<std::string> result;
		for (auto proxy : *this) {
			if (proxy->isExecutable()){
				for (auto rule : proxy->rules) {
					if (rule->isVisible && rule->type == Gc::Model::ListCfg::Rule::NORMAL) {
						result.push_back(rule->outputName);
					}
				}
			}
		}
		return result;
	}

	public: static std::list<Gc::Model::ListCfg::ProxylistItem> generateEntryTitleList(
		std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>> const& parent,
		std::string const& labelPathPrefix,
		std::string const& numericPathPrefix,
		std::string const& numericPathLabelPrefix,
		int* offset = nullptr
	) {
		std::list<Gc::Model::ListCfg::ProxylistItem> result;
		int i = (offset != nullptr ? *offset : 0);
		for (auto rule : parent){
			if (rule->isVisible && (rule->type == Gc::Model::ListCfg::Rule::NORMAL || rule->type == Gc::Model::ListCfg::Rule::SUBMENU)) {
				std::ostringstream currentNumPath;
				currentNumPath << numericPathPrefix << i;
				std::ostringstream currentLabelNumPath;
				currentLabelNumPath << numericPathLabelPrefix << (i+1);

				bool addedSomething = true;
				if (rule->type == Gc::Model::ListCfg::Rule::SUBMENU) {
					std::list<Gc::Model::ListCfg::ProxylistItem> subList = Gc::Model::ListCfg::Proxylist::generateEntryTitleList(
						rule->subRules,
						labelPathPrefix + rule->outputName + ">",
						currentNumPath.str() + ">",
						currentLabelNumPath.str() + ">"
					);
					if (subList.size() == 0) {
						addedSomething = false;
					}
					result.splice(result.end(), subList);
				} else {
					Gc::Model::ListCfg::ProxylistItem newItem;
					newItem.labelPathLabel = labelPathPrefix + rule->outputName;
					newItem.labelPathValue = labelPathPrefix + rule->outputName;
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

	public: std::shared_ptr<Gc::Model::ListCfg::Proxy> getProxyByRule(
		std::shared_ptr<Gc::Model::ListCfg::Rule> rule,
		std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>> const& list,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> parentProxy
	) {
		for (auto loop_rule : list) {
			if (loop_rule == rule)
				return parentProxy;
			else {
				try {
					return this->getProxyByRule(rule, loop_rule->subRules, parentProxy);
				} catch (Gc::Common::ItemNotFoundException const& e) {
					// do nothing
				}
			}
		}
		throw Gc::Common::ItemNotFoundException("proxy by rule not found", __FILE__, __LINE__);
	}

	public: std::shared_ptr<Gc::Model::ListCfg::Proxy> getProxyByRule(std::shared_ptr<Gc::Model::ListCfg::Rule> rule) {
		for (auto proxy : *this) {
			try {
				return this->getProxyByRule(rule, proxy->rules, proxy);
			} catch (Gc::Common::ItemNotFoundException const& e) {
				// do nothing
			}
		}
		throw Gc::Common::ItemNotFoundException("proxy by rule not found", __FILE__, __LINE__);
	}

	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>>::iterator moveRuleToNewProxy(
		std::shared_ptr<Gc::Model::ListCfg::Rule> rule,
		int direction,
		std::shared_ptr<Gc::Model::ListCfg::Script> dataSource = nullptr
	) {
		auto currentProxy = this->getProxyByRule(rule);
		auto proxyIter = this->begin();

		for (auto proxy : *this) {
			if (proxy == currentProxy) {
				break;
			}
			proxyIter++;
		}
	
		if (direction == 1) {
			proxyIter++;
		}
		if (dataSource == nullptr) {
			dataSource = currentProxy->dataSource;
		}
		auto newProxy = *this->insert(proxyIter, std::make_shared<Gc::Model::ListCfg::Proxy>(dataSource, false));
		newProxy->removeEquivalentRules(rule);
		auto movedRule = newProxy->rules.insert(
			direction == -1 ? newProxy->rules.end() : newProxy->rules.begin(),
			std::make_shared<Gc::Model::ListCfg::Rule>(*rule)
		);
		rule->setVisibility(false);
	
		if (!currentProxy->hasVisibleRules()) {
			this->deleteProxy(currentProxy);
		}
		return movedRule;
	}

	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>>::iterator getNextVisibleRule(std::shared_ptr<Gc::Model::ListCfg::Rule> base, int direction) {
		auto proxy = this->getProxyByRule(base);
		auto iter = proxy->getListIterator(base, proxy->getRuleList(proxy->getParentRule(base, nullptr)));
		return this->getNextVisibleRule(iter, direction);
	}

	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>>::iterator getNextVisibleRule(
		std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>>::iterator base,
		int direction
	) {
		auto proxyIter = this->getIter(this->getProxyByRule(*base));

		bool hasParent = false;
		if (proxyIter->get()->getParentRule(*base)) {
			hasParent = true;
		}

		while (proxyIter != this->end()) {
			try {
				return proxyIter->get()->getNextVisibleRule(base, direction);
			} catch (Gc::Common::NoMoveTargetException const& e) {

				if (hasParent) {
					throw Gc::Common::NoMoveTargetException("next visible rule not found", __FILE__, __LINE__);
				}

				if (direction == 1) {
					proxyIter++;
					if (proxyIter == this->end()) {
						throw Gc::Common::NoMoveTargetException("next visible rule not found", __FILE__, __LINE__);
					}
					base = proxyIter->get()->rules.begin();
				} else {
					proxyIter--;
					if (proxyIter == this->end()) {
						throw Gc::Common::NoMoveTargetException("next visible rule not found", __FILE__, __LINE__);
					}
					base = proxyIter->get()->rules.end();
					base--;
				}
				if (base->get()->isVisible) {
					return base;
				}
			}
		}
		throw Gc::Common::NoMoveTargetException("next visible rule not found", __FILE__, __LINE__);
	}

	std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>>::iterator getIter(std::shared_ptr<Gc::Model::ListCfg::Proxy> proxy) {
		auto iter = this->begin();
		while (iter != this->end()) {
			if (*iter == proxy) {
				break;
			}
			iter++;
		}
		return iter;
	}

	void splitProxy(std::shared_ptr<Gc::Model::ListCfg::Proxy> proxyToSplit, std::shared_ptr<Gc::Model::ListCfg::Rule> firstRuleOfPart2, int direction) {
		auto iter = this->getIter(proxyToSplit);
		auto sourceProxy = *iter;
		if (direction == 1) {
			iter++;
		}
		auto newProxy = *this->insert(iter, std::make_shared<Gc::Model::ListCfg::Proxy>(sourceProxy->dataSource, false));
	
		bool isSecondPart = false;
		if (direction == 1) {
			for (auto ruleIter = sourceProxy->rules.begin(); ruleIter != sourceProxy->rules.end(); ruleIter++) {
				if (*ruleIter == firstRuleOfPart2) {
					isSecondPart = true;
				}
				if (isSecondPart) {
					newProxy->removeEquivalentRules(*ruleIter);
					newProxy->rules.push_back(*ruleIter);
					ruleIter->get()->isVisible = false;
				}
			}
		} else {
			for (auto ruleIter = sourceProxy->rules.rbegin(); ruleIter != sourceProxy->rules.rend(); ruleIter++) {
				if (*ruleIter == firstRuleOfPart2) {
					isSecondPart = true;
				}
				if (isSecondPart) {
					newProxy->removeEquivalentRules(*ruleIter);
					newProxy->rules.push_front(*ruleIter);
					ruleIter->get()->isVisible = false;
				}
			}
		}
	}

	std::shared_ptr<Gc::Model::ListCfg::Rule> getVisibleRuleForEntry(std::shared_ptr<Gc::Model::ListCfg::Entry> entry) {
		for (auto proxy : *this) {
			if (proxy->isExecutable()) {
				std::shared_ptr<Gc::Model::ListCfg::Rule> result = proxy->getVisibleRuleForEntry(entry);
				if (result) {
					return result;
				}
			}
		}
		return NULL;
	}

	bool hasConflicts() const {
		std::map<std::string, bool> resources; // key: combination of number, "_" and name. Value: true if used before
		for (auto proxy : *this) {
			assert(proxy->dataSource); // assume all proxies are having a datasource
			std::ostringstream resourceName;
			resourceName << proxy->index << "_" << proxy->dataSource->name;
			if (resources[resourceName.str()]) {
				return true;
			} else {
				resources[resourceName.str()] = true;
			}
		}
		return false;
	}

	bool hasProxy(std::shared_ptr<Gc::Model::ListCfg::Proxy> proxy) {
		for (auto proxy_loop : *this) {
			if (proxy_loop == proxy) {
				return true;
			}
		}
		return false;
	}

	operator Gc::Common::ArrayStructure::Container() const {
		Gc::Common::ArrayStructure::Container result;
		int trashIterPos = 0;
		result["trash"].isArray = true;
		for (auto trashedProxy : this->trash) {
			result["trash"][trashIterPos] = Gc::Common::ArrayStructure::Container(*trashedProxy);
			trashIterPos++;
		}
		int itemsIterPos = 0;
		result["(items)"].isArray = true;
		for (auto proxy : *this) {
			result["(items)"][itemsIterPos] = Gc::Common::ArrayStructure::Container(*proxy);
			itemsIterPos++;
		}
		return result;
	}

};}}}

#endif
