#ifndef GRUBCFG_LIBPROXY_H_INCLUDED
#define GRUBCFG_LIBPROXY_H_INCLUDED
#include <string>
#include <list>
#include "confparser.h"

struct ListRule {
	enum RuleType {
		NORMAL, OTHER_ENTRIES_PLACEHOLDER
	} type;
	bool isVisible;
	std::string name;
	std::string outputName;
	ListRule(RuleType type, std::string name, bool isVisible);
};

std::list<ListRule> readRuleString(const char* ruleString);

EntryList executeListRules(EntryList oldList, std::list<ListRule> const& listRules);

#endif
