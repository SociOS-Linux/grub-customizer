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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef SCRIPTSOURCEMAP_H_
#define SCRIPTSOURCEMAP_H_

#include <map>
#include <string>
#include "Env.h"
#include "../lib/csv.h"
#include <dirent.h>
#include "../lib/assert.h"

class ScriptSourceMap : public std::map<std::string, std::string> {
	Model_Env& env;
	std::string _getFilePath();
public:
	ScriptSourceMap(Model_Env& env);
	void load();
	void update();
	void save();
};


#endif /* SCRIPTSOURCEMAP_H_ */
