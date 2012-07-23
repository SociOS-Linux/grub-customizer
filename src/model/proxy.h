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
#include <map>
#include "entryPathBuilderImpl.h"

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
	std::map<Script*, std::list<std::list<std::string> > > __idPathList; //to be used by sync();
	std::map<Script*, std::list<std::list<std::string> > > __idPathList_OtherEntriesPlaceHolders; //to be used by sync();
	Proxy();
	Proxy(Script& dataSource);
	bool isExecutable() const;
	void set_isExecutable(bool value);
	static std::list<Rule> parseRuleString(const char** ruleString);
	void importRuleString(const char* ruleString);
	Rule* getRuleByEntry(Entry const& entry, std::list<Rule>& list, Rule::RuleType ruletype);
	void unsync(Rule* parent = NULL);
	bool sync(bool deleteInvalidRules = true, bool expand = true, std::map<std::string, Script*> scriptMap = std::map<std::string, Script*>());
	void sync_connectExisting(Rule* parent = NULL, std::map<std::string, Script*> scriptMap = std::map<std::string, Script*>());
	void sync_connectExistingByHash(Rule* parent = NULL, std::map<std::string, Script*> scriptMap = std::map<std::string, Script*>());
	void sync_add_placeholders(Rule* parent = NULL, std::map<std::string, Script*> scriptMap = std::map<std::string, Script*>());
	void sync_expand(std::map<std::string, Script*> scriptMap = std::map<std::string, Script*>());
	void sync_cleanup(Rule* parent = NULL, std::map<std::string, Script*> scriptMap = std::map<std::string, Script*>());
	bool isModified(Rule const* parentRule = NULL, Entry const* parentEntry = NULL) const;
	bool deleteFile();
	std::list<std::string> getScriptList(std::map<Entry const*, Script const*> const& entrySourceMap, std::map<Script const*, std::string> const& scriptTargetMap) const;
	bool generateFile(std::string const& path, int cfg_dir_prefix_length, std::string const& cfg_dir_noprefix, std::map<Entry const*, Script const*> ruleSourceMap, std::map<Script const*, std::string> const& scriptTargetMap); //before running this function, the realted script file must be saved!
	std::string getScriptName();
	Rule& moveRule(Rule* rule, int direction);
	Rule* removeSubmenu(Rule* childItem);
	Rule* createSubmenu(Rule* childItem);
	bool ruleIsFromOwnScript(Rule const& rule) const;
	void removeForeignChildRules(Rule& parent);
private:
	Rule* getParentRule(Rule* child, Rule* root = NULL);
	std::list<Rule>& getRuleList(Rule* parentElement);
	std::list<Rule>::iterator getListIterator(Rule const& needle, std::list<Rule>& haystack);
	static void adjustIterator(std::list<Rule>::iterator& iter, int adjustment);
};

#endif
