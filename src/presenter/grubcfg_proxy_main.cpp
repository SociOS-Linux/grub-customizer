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

#include "../model/script.h"
#include "../model/proxy.h"
#include "../model/entry.h"
#include "../model/rule.h"
#include "../model/grublistCfg.h" // multi

#include <iostream>

int main(int argc, char** argv){
	if (argc == 2) {
		Script script("noname", "");
		Entry newEntry;
		std::string plaintextBuffer;
		while (newEntry = Entry(stdin, GrubConfRow(), NULL, &plaintextBuffer)) {
			script.entries().push_back(newEntry);
		}
		if (plaintextBuffer.size()) {
			script.entries().push_front(Entry("#text", "", plaintextBuffer, Entry::PLAINTEXT));
		}
		
		Proxy proxy;
		proxy.importRuleString(argv[1]);

		proxy.dataSource = &script;
		proxy.sync(true, true);
		
		for (std::list<Rule>::iterator iter = proxy.rules.begin(); iter != proxy.rules.end(); iter++){
			iter->print(std::cout);
		}
		return 0;
	} else if (argc == 3 && std::string(argv[2]) == "multi") {
		GrubEnv env;
		GrublistCfg scriptSource(env);
		scriptSource.ignoreLock = true;
		{ // this scope prevents access to the unused proxy variable - push_back takes a copy!
			Proxy proxy;
			proxy.importRuleString(argv[1]);
			scriptSource.proxies.push_back(proxy);
		}
		scriptSource.readGeneratedFile(stdin, true, false);

		scriptSource.proxies.front().dataSource = &scriptSource.repository.front(); // the first Script is always the main script

		std::map<std::string, Script*> map = scriptSource.repository.getScriptPathMap();
		scriptSource.proxies.front().sync(true, true, map);

		for (std::list<Rule>::iterator iter = scriptSource.proxies.front().rules.begin(); iter != scriptSource.proxies.front().rules.end(); iter++){
			iter->print(std::cout);
		}
	} else {
		std::cerr << "wrong argument count. You have to give the config as parameter 1!" << std::endl;
		return 1;
	}
}
