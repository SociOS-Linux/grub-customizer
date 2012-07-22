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

#ifndef SETTINGSSTORE_H_
#define SETTINGSSTORE_H_

#include <list>
#include <string>
#include <cstdio>

struct SettingRow {
	SettingRow();
	std::string name, value, comment, plaintext;
	bool hasExportPrefix, isActive, isSetting;
	void validate();
	std::string getOutput();
};

class SettingsStore {
	std::string filePath;
protected:
	std::list<SettingRow> settings;
public:
	SettingsStore(FILE* source = NULL);
	std::list<SettingRow>::iterator begin(bool jumpOverPlaintext = true);
	std::list<SettingRow>::iterator end();
	void iter_to_next_setting(std::list<SettingRow>::iterator& iter);
	std::string getValue(std::string const& name);
	bool setValue(std::string const& name, std::string const& value);
	std::string addNewItem();
	void removeItem(std::string const& name);
	void renameItem(std::string const& old_name, std::string const& new_name);
	bool isActive(std::string const& name, bool checkValueToo = false);
	bool setIsActive(std::string const& name, bool value);
	bool setIsExport(std::string const& name, bool isExport);
	void load(FILE* source);
	void clear();
};

#endif /* SETTINGSSTORE_H_ */
