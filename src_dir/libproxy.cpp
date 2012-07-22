#include "libproxy.h"

ListRule::ListRule(RuleType type, std::string name, bool isVisible)
	: type(type), name(name), isVisible(isVisible), outputName(name)
{}

std::list<ListRule> readRuleString(const char* ruleString){
	std::list<ListRule> result;
	
	bool inString = false, inAlias = false;
	std::string name;
	bool visible = false;
	for (const char* iter = ruleString; *iter; iter++) {
		if (!inString && *iter == '+')
			visible = true;
		else if (!inString && *iter == '-')
			visible = false;
		else if (*iter == '\'' && iter[1] != '\''){
			inString = !inString;
			if (!inString){
				if (inAlias)
					result.back().outputName = name;
				else
					result.push_back(ListRule(ListRule::NORMAL, name, visible));
				inAlias = false;
			}
			name = "";
		}
		else if (!inString && *iter == '*')
			result.push_back(ListRule(ListRule::OTHER_ENTRIES_PLACEHOLDER, "*", visible));

		else if (inString){
			name += *iter;
			if (*iter == '\'')
				iter++;
		}
		else if (!inString && *iter == 'a' && *++iter == 's')
			inAlias = true;
	}
	
	return result;
}

EntryList executeListRules(EntryList oldList, std::list<ListRule> const& listRules){
	EntryList newList;
	newList.other_entries_pos = -1;
	newList.other_entries_visible = false;
	
	//moving or deleting the old list entries
	for (std::list<ListRule>::const_iterator iter = listRules.begin(); iter != listRules.end(); iter++){
		if (iter->type == ListRule::NORMAL){
			std::list<Entry>::iterator entryToMove = oldList.getEntryIterByName(iter->name);
			if (entryToMove != oldList.end()){
				entryToMove->outputName = iter->outputName;
				entryToMove->disabled = !iter->isVisible;
				newList.push_back(*entryToMove);
				
				oldList.erase(entryToMove);
			}
		}
		else if (iter->type == ListRule::OTHER_ENTRIES_PLACEHOLDER){
			newList.other_entries_pos = newList.size();
			if (iter->isVisible)
				newList.other_entries_visible = true;
		}
	}

	//if there is no "+*" or "-*" assume -* to the end
	if (newList.other_entries_pos == -1){
		newList.other_entries_pos = newList.size();
	}
	//inserting the rule-less entries (selected by +*)
	
	if (newList.other_entries_pos != -1){
		for (std::list<Entry>::iterator iter = oldList.begin(); iter != oldList.end(); iter++){
			iter->outputName = iter->name;
			if (!newList.other_entries_visible)
				iter->disabled = true;
		}
		
		std::list<Entry>::iterator other_entries_iter = newList.begin();
		for (int i = 0; i < newList.other_entries_pos; i++)
			other_entries_iter++;

		newList.splice(other_entries_iter, oldList);
	}
	return newList;
}

