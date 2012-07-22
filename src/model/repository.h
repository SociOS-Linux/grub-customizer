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

#ifndef GRUB_CUSTOMIZER_REPOSITORY_INCLUDED
#define GRUB_CUSTOMIZER_REPOSITORY_INCLUDED
#include "script.h"
#include "libproxyscript_parser.h"
#include "pscriptname_translator.h"
#include <sys/stat.h>
#include <dirent.h>

struct Repository : public std::list<Script> {
	void load(std::string const& directory, bool is_proxifiedScript_dir);
	Script* getScriptByFilename(std::string const& fileName, bool createScriptIfNotFound = false);
	Script* getNthScript(int pos);
	void deleteAllEntries();
};

#endif
