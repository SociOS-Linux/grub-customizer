#ifndef GRUB_CUSTOMIZER_PROXY_INCLUDED
#define GRUB_CUSTOMIZER_PROXY_INCLUDED
#include "rule.h"
#include "script.h"
#include <iostream>
#include <sys/stat.h>

struct Proxy {
	std::list<Rule> rules;
	int index;
	short int permissions;
	std::string fileName; //may be the same as Script::fileName
	Script* dataSource;
	Proxy();
	Proxy(Script& dataSource);
	bool isExecutable() const;
	void set_isExecutable(bool value);
	void importRuleString(const char* ruleString);
	bool sync(bool deleteInvalidRules = true, bool expand = true);
	bool isModified() const;
	bool deleteFile();
	bool generateFile(std::string const& path, int cfg_dir_prefix_length, std::string const& cfg_dir_noprefix); //before running this function, the realted script file must be saved!
	std::string getScriptName();
};

#endif
