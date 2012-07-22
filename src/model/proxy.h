/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GRUB_CUSTOMIZER_PROXY_INCLUDED
#define GRUB_CUSTOMIZER_PROXY_INCLUDED
#include "rule.h"
#include "script.h"
#include <sys/stat.h>
#include <assert.h>

struct Proxy {
	enum Exception {
		RULE_NOT_FOUND,
		NO_MOVE_TARGET_FOUND
	};
	std::list<Rule> rules;
	int index;
	short int permissions;
	std::string fileName; //may be the same as Script::fileName
	Script* dataSource;
	std::list<std::list<std::string> > __idPathList; //to be used by sync();
	std::list<std::list<std::string> > __idPathList_OtherEntriesPlaceHolders; //to be used by sync();
	Proxy();
	Proxy(Script& dataSource);
	bool isExecutable() const;
	void set_isExecutable(bool value);
	static std::list<Rule> parseRuleString(const char** ruleString);
	void importRuleString(const char* ruleString);
	Rule* getRuleByEntry(Entry const& entry, std::list<Rule>& list, Rule::RuleType ruletype);
	bool sync(bool deleteInvalidRules = true, bool expand = true);
	void sync_connectExisting(Rule* parent = NULL);
	void sync_add_placeholders(Rule* parent = NULL);
	void sync_expand();
	void sync_cleanup(Rule* parent = NULL);
	bool isModified(Rule const* parent = NULL) const;
	bool deleteFile();
	bool generateFile(std::string const& path, int cfg_dir_prefix_length, std::string const& cfg_dir_noprefix); //before running this function, the realted script file must be saved!
	std::string getScriptName();
	Rule& moveRule(Rule* rule, int direction);
private:
	Rule* getParentRule(Rule* child, Rule* root = NULL);
	std::list<Rule>& getRuleList(Rule* parentElement);
	std::list<Rule>::iterator getListIterator(Rule const& needle, std::list<Rule>& haystack);
	Rule* getPlaceholderBySourceList(std::list<Entry> const& sourceList, std::list<Rule>& baseList);
	static void adjustIterator(std::list<Rule>::iterator& iter, int adjustment);
};

#endif
