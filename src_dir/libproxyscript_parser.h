#ifndef GRUB_CFG_LIBPROXYSCRIPT_PARSER_INCLUDED
#define GRUB_CFG_LIBPROXYSCRIPT_PARSER_INCLUDED
#include <cstdio>
#include <string>

struct ProxyscriptData {
	std::string scriptCmd, proxyCmd, ruleString;
};

ProxyscriptData parseProxyScript(FILE* fpProxyScript);
#endif
