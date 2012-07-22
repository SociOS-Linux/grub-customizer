#ifndef GRUB_CFG_LIBProxyScriptData_PARSER_INCLUDED
#define GRUB_CFG_LIBProxyScriptData_PARSER_INCLUDED
#include <cstdio>
#include <string>
#include <iostream>

struct ProxyScriptData {
	std::string scriptCmd, proxyCmd, ruleString;
	bool is_valid;
	ProxyScriptData(FILE* fpProxyScript);
	bool load(FILE* fpProxyScript);
	static bool is_proxyscript(FILE* fpProxyScript);
	static bool is_proxyscript(std::string const& filePath);
	operator bool();
};

#endif
