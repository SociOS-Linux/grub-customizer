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

#ifndef GRUB_CFG_LIBProxyScriptData_PARSER_INCLUDED
#define GRUB_CFG_LIBProxyScriptData_PARSER_INCLUDED
#include <cstdio>
#include <string>

struct Model_ProxyScriptData {
	std::string scriptCmd, proxyCmd, ruleString;
	bool is_valid;
	Model_ProxyScriptData(FILE* fpProxyScript);
	bool load(FILE* fpProxyScript);
	static bool is_proxyscript(FILE* fpProxyScript);
	static bool is_proxyscript(std::string const& filePath);
	operator bool();
};

#endif
