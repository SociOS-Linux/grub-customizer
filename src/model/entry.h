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

#ifndef GRUB_CUSTOMIZER_ENTRY_INCLUDED
#define GRUB_CUSTOMIZER_ENTRY_INCLUDED
#include <cstdio>
#include <string>
#include <list>
#include "../presenter/commonClass.h"

struct GrubConfRow {
	GrubConfRow(FILE* sourceFile);
	GrubConfRow();
	std::string text;
	bool eof;
	bool is_loaded;
	operator bool();
};

std::string str_replace(const std::string &search, const std::string &replace, std::string subject);

struct Entry : public CommonClass {
	enum EntryType {
		MENUENTRY,
		SUBMENU,
		ROOT_ENTRY,
		PLAINTEXT
	} type;
	bool isValid;
	std::string name, extension, content;
	std::list<Entry> subEntries;
	Entry();
	Entry(std::string name, std::string extension, std::string content = "", EntryType type = MENUENTRY);
	Entry(FILE* sourceFile, GrubConfRow firstRow = GrubConfRow(), Logger* logger = NULL, std::string* plaintextBuffer = NULL);
	std::list<Entry>& getSubEntries();
	operator bool() const;
};

#endif
