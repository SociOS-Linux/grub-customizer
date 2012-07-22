#include <iostream>
#include "libproxyscript_parser.h"



int main(int argc, char** argv){
	if (argc == 2){
		FILE* fpProxyScript = fopen(argv[1], "r");
		
		ProxyscriptData data = parseProxyScript(fpProxyScript);
		fclose(fpProxyScript);
		
		std::cout << "Script cmd: " << data.scriptCmd << std::endl;
		std::cout << "Proxy cmd: " << data.proxyCmd << std::endl;
		std::cout << "Rule string: " << data.ruleString << std::endl;
	}
	else {
		std::cerr << "you have to give the proxy script path as parameter 1" << std::endl;
	}
}

