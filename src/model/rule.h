#ifndef GRUB_CUSTOMIZER_RULE_INCLUDED
#define GRUB_CUSTOMIZER_RULE_INCLUDED
#include <string>
#include "entry.h"
#include <iostream>

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
