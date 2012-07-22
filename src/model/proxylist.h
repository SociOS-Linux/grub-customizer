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

#ifndef GRUB_CUSTOMIZER_PROXYLIST_INCLUDED
#define GRUB_CUSTOMIZER_PROXYLIST_INCLUDED
#include <list>
#include "proxy.h"
struct ProxyList : public std::list<Proxy> {
	enum Exception {
		NO_RELATED_PROXY_FOUND
	};
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
	Proxy* getProxyByRule(Rule* rule, std::list<Rule> const& list, Proxy& parentProxy);
	Proxy* getProxyByRule(Rule* rule);
};

#endif
