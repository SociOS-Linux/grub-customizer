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
	static std::list<Rule> parseRuleString(const char** ruleString);
	void importRuleString(const char* ruleString);
	bool sync(bool deleteInvalidRules = true, bool expand = true);
	bool isModified() const;
	bool deleteFile();
	bool generateFile(std::string const& path, int cfg_dir_prefix_length, std::string const& cfg_dir_noprefix); //before running this function, the realted script file must be saved!
	std::string getScriptName();
};

#endif
