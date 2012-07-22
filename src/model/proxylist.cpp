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

#include "proxylist.h"

std::list<Proxy*> ProxyList::getProxiesByScript(Script const& script) {
	std::list<Proxy*> result;
	for (ProxyList::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->dataSource == &script)
			result.push_back(&*iter);
	}
	return result;
}
std::list<const Proxy*> ProxyList::getProxiesByScript(Script const& script) const {
	std::list<const Proxy*> result;
	for (ProxyList::const_iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->dataSource == &script)
			result.push_back(&*iter);
	}
	return result;
}
void ProxyList::sync_all(bool deleteInvalidRules, bool expand, Script* relatedScript){ //relatedScript = NULL: sync all proxies, otherwise only sync proxies wich target the given Script
	for (ProxyList::iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
		if (relatedScript == NULL || proxy_iter->dataSource == relatedScript)
			proxy_iter->sync(deleteInvalidRules, expand);
	}	
}

void ProxyList::unsync_all() {
	for (ProxyList::iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
		proxy_iter->unsync();
	}
}

bool ProxyList::proxyRequired(Script const& script) const {
	std::list<const Proxy*> plist = this->getProxiesByScript(script);
	if (plist.size() == 1){
		bool res = plist.front()->isModified();
		return plist.front()->isModified();
	}
	else
		return true;
}

void ProxyList::deleteAllProxyscriptFiles(){
	for (ProxyList::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->dataSource && iter->dataSource->fileName != iter->fileName){
			iter->deleteFile();
		}
	}
}

bool ProxyList::compare_proxies(Proxy const& a, Proxy const& b){
	return a.index < b.index;
}

void ProxyList::sort(){
	std::list<Proxy>::sort(ProxyList::compare_proxies);
}

void ProxyList::deleteProxy(Proxy* proxyPointer){
	for (ProxyList::iterator iter = this->begin(); iter != this->end(); iter++){
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

void ProxyList::clearTrash(){
	for (std::list<Proxy>::iterator iter = this->trash.begin(); iter != this->trash.end(); iter++){
		iter->deleteFile();
	}
}

std::list<EntryTitleListItem> ProxyList::generateEntryTitleList() const {
	std::list<EntryTitleListItem> result;
	int offset = 0;
	for (ProxyList::const_iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
		if (proxy_iter->isExecutable()){
			std::list<EntryTitleListItem> subList = ProxyList::generateEntryTitleList(proxy_iter->rules, "", "", "", &offset);
			result.splice(result.end(), subList);
		}
	}
	return result;
}

std::list<std::string> ProxyList::getToplevelEntryTitles() const {
	std::list<std::string> result;
	for (ProxyList::const_iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
		if (proxy_iter->isExecutable()){
			for (std::list<Rule>::const_iterator rule_iter = proxy_iter->rules.begin(); rule_iter != proxy_iter->rules.end(); rule_iter++) {
				if (rule_iter->isVisible && rule_iter->type == Rule::NORMAL) {
					result.push_back(rule_iter->outputName);
				}
			}
		}
	}
	return result;
}

std::list<EntryTitleListItem> ProxyList::generateEntryTitleList(std::list<Rule> const& parent, std::string const& labelPathPrefix, std::string const& numericPathPrefix, std::string const& numericPathLabelPrefix, int* offset) {
	std::list<EntryTitleListItem> result;
	int i = (offset != NULL ? *offset : 0);
	for (std::list<Rule>::const_iterator rule_iter = parent.begin(); rule_iter != parent.end(); rule_iter++){
		if (rule_iter->isVisible && (rule_iter->type == Rule::NORMAL || rule_iter->type == Rule::SUBMENU)) {
			std::ostringstream currentNumPath;
			currentNumPath << numericPathPrefix << i;
			std::ostringstream currentLabelNumPath;
			currentLabelNumPath << numericPathLabelPrefix << (i+1);

			if (rule_iter->type == Rule::SUBMENU) {
				std::list<EntryTitleListItem> subList = ProxyList::generateEntryTitleList(rule_iter->subRules, labelPathPrefix + rule_iter->outputName + ">", currentNumPath.str() + ">", currentLabelNumPath.str() + ">");
				result.splice(result.end(), subList);
			} else {
				EntryTitleListItem newItem;
				newItem.labelPathLabel = rule_iter->outputName;
				newItem.labelPathValue = labelPathPrefix + rule_iter->outputName;
				newItem.numericPathLabel = currentLabelNumPath.str();
				newItem.numericPathValue = currentNumPath.str();
				result.push_back(newItem);
			}
			i++;
		}
	}
	if (offset != NULL) {
		*offset = i;
	}
	return result;
}

Proxy* ProxyList::getProxyByRule(Rule* rule, std::list<Rule> const& list, Proxy& parentProxy) {
	for (std::list<Rule>::const_iterator rule_iter = list.begin(); rule_iter != list.end(); rule_iter++){
		if (&*rule_iter == rule)
			return &parentProxy;
		else {
			try {
				return this->getProxyByRule(rule, rule_iter->subRules, parentProxy);
			} catch (ProxyList::Exception e) {
				if (e != NO_RELATED_PROXY_FOUND)
					throw e;
			}
		}
	}
	throw NO_RELATED_PROXY_FOUND;
}

Proxy* ProxyList::getProxyByRule(Rule* rule) {
	for (ProxyList::iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
		try {
			return this->getProxyByRule(rule, proxy_iter->rules, *proxy_iter);
		} catch (ProxyList::Exception e) {
			if (e != NO_RELATED_PROXY_FOUND)
				throw e;
		}
	}
	throw NO_RELATED_PROXY_FOUND;
}

