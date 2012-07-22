#ifndef GRUB_CUSTOMIZER_PROXYLIST_INCLUDED
#define GRUB_CUSTOMIZER_PROXYLIST_INCLUDED
#include <list>
#include "proxy.h"
struct ProxyList : public std::list<Proxy> {
	std::list<Proxy> trash; //removed proxies
	std::list<Proxy*> getProxiesByScript(Script const& script);
	std::list<const Proxy*> getProxiesByScript(Script const& script) const;
	void sync_all(bool deleteInvalidRules = true, bool expand = true, Script* relatedScript = NULL); //relatedScript = NULL: sync all proxies, otherwise only sync proxies wich target the given Script
	bool proxyRequired(Script const& script) const;
	void deleteAllProxyscriptFiles();
	static bool compare_proxies(Proxy const& a, Proxy const& b);
	void sort();
	void deleteProxy(Proxy* proxyPointer);
	void clearTrash();
	std::list<std::string> generateEntryTitleList() const;
	Proxy* getProxyByRule(Rule* rule);
};

#endif
