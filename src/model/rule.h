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

#ifndef GRUB_CUSTOMIZER_RULE_INCLUDED
#define GRUB_CUSTOMIZER_RULE_INCLUDED
#include <string>
#include "entry.h"
#include "../interface/entryPathBuilder.h"
#include "../interface/entryPathFollower.h"
#include <iostream>

struct Rule {
	Entry* dataSource; //assigned when using RuleType::OTHER_ENTRIES_PLACEHOLDER
	std::list<Entry>* dataSource_list; //assigned when using RuleType::OTHER_ENTRIES_PLACEHOLDER
	std::string outputName;
	std::list<std::string> __idpath; //should only be used by sync()!
	bool isVisible;
	std::list<Rule> subRules;
	enum RuleType {
		NORMAL, OTHER_ENTRIES_PLACEHOLDER
	} type;
	Rule(RuleType type, std::list<std::string> path, std::string outputName, bool isVisible);
	Rule(RuleType type, std::list<std::string> path, bool isVisible);
	Rule(Entry& source, bool isVisible, EntryPathFollower& pathFollower, std::list<std::list<std::string> > const& pathesToIgnore = std::list<std::list<std::string> >(), std::list<std::string> const& currentPath = std::list<std::string>()); //generate rule for given entry
	Rule();
	std::string toString(EntryPathBilder const& pathBuilder);
	void print() const;
	std::string getEntryName() const;
};

#endif
