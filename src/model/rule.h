#ifndef GRUB_CUSTOMIZER_RULE_INCLUDED
#define GRUB_CUSTOMIZER_RULE_INCLUDED
#include <string>
#include "entry.h"
#include <iostream>

std::string str_replace(const std::string &search, const std::string &replace, std::string subject);

struct Rule {
	Entry* dataSource;
	std::string outputName;
	std::string __idname; //should only be used by sync()!
	bool isVisible;
	enum RuleType {
		NORMAL, OTHER_ENTRIES_PLACEHOLDER
	} type;
	Rule(RuleType type, std::string name, bool isVisible);
	Rule(Entry& source, bool isVisible); //generate rule for given entry
	operator std::string();
	void print() const;
	std::string getEntryName() const;
};

#endif
