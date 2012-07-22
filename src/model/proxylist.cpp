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

std::list<std::string> ProxyList::generateEntryTitleList() const {
	std::list<std::string> result;
	for (ProxyList::const_iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
		if (proxy_iter->isExecutable()){
			for (std::list<Rule>::const_iterator rule_iter = proxy_iter->rules.begin(); rule_iter != proxy_iter->rules.end(); rule_iter++){
				if (rule_iter->isVisible && rule_iter->type != Rule::OTHER_ENTRIES_PLACEHOLDER)
					result.push_back(rule_iter->outputName);
			}
		}
	}
	return result;
}

Proxy* ProxyList::getProxyByRule(Rule* rule) {
	for (ProxyList::iterator proxy_iter = this->begin(); proxy_iter != this->end(); proxy_iter++){
		for (std::list<Rule>::iterator rule_iter = proxy_iter->rules.begin(); rule_iter != proxy_iter->rules.end(); rule_iter++){
			if (&*rule_iter == rule)
				return &*proxy_iter;
		}
	}
	throw NO_RELATED_PROXY_FOUND;
}

