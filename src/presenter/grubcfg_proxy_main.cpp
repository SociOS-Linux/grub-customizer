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

int main(int argc, char** argv){
	if (argc == 2){
		Script script("noname", "");
		Entry newEntry;
		while (newEntry = Entry(stdin)){
			script.push_back(newEntry);
		}
		
		Proxy proxy;
		proxy.importRuleString(argv[1]);
		proxy.dataSource = &script;
		proxy.sync(true, true);
		
		for (std::list<Rule>::iterator iter = proxy.rules.begin(); iter != proxy.rules.end(); iter++){
			iter->print();
		}
		return 0;
	}
	else {
		std::cerr << "wrong argument count. You have to give the config as parameter 1!" << std::endl;
		return 1;
	}
}
