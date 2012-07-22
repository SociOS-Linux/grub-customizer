#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <list>
#include "libproxy.h"

int main(int argc, char** argv){ //TODO: Funktionalität teilweise auslagern
	if (argc == 2){
		EntryList oldList(stdin); //reading the script output
	
		//reading the config
		std::list<ListRule> listRules = readRuleString(argv[1]);
	
		
		EntryList newList = executeListRules(oldList, listRules);
		
		
		//printing the new configuration
		for (std::list<Entry>::iterator iter = newList.begin(); iter != newList.end(); iter++){
			if (!iter->disabled)
				std::cout << "menuentry \""+iter->outputName+"\""+iter->extension+"{\n"+iter->content+"}\n";
		}
		return 0;
	}
	else {
		std::cerr << "wrong argument count. You have to give the config as parameter 1!" << std::endl;
		return 1;
	}
}









