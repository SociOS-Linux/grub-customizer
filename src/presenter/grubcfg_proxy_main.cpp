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
