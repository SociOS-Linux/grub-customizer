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

#ifndef SETTINGSSTORE_H_
#define SETTINGSSTORE_H_

#include <list>
#include <string>
#include <cstdio>

namespace Gc { namespace Model { class SettingsStore
{
	private: std::string filePath;
	protected: std::list<Gc::Model::SettingsStoreRow> settings;

	public: SettingsStore(FILE* source = nullptr)
	{
		if (source) {
			this->load(source);
		}
	}

	public: std::list<Gc::Model::SettingsStoreRow>::iterator begin(bool jumpOverPlaintext = true)
	{
		std::list<Gc::Model::SettingsStoreRow>::iterator iter = settings.begin();
		if (!iter->isSetting && jumpOverPlaintext)
			iter_to_next_setting(iter);
		return iter;
	}

	public: std::list<Gc::Model::SettingsStoreRow>::iterator end()
	{
		return settings.end();
	}

	public: void iter_to_next_setting(std::list<Gc::Model::SettingsStoreRow>::iterator& iter)
	{
		iter++;
		while (iter != settings.end()){
			if (iter->isSetting)
				break;
			else
				iter++;
		}
	}

	public: std::string getValue(std::string const& name)
	{
		for (std::list<Gc::Model::SettingsStoreRow>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
			if (name == iter->name)
				return iter->value;
		}
		return "";
	}

	public: bool setValue(std::string const& name, std::string const& value)
	{
		for (std::list<Gc::Model::SettingsStoreRow>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
			if (name == iter->name){
				if (iter->value != value){ //only set when the new value is really new
					iter->value = value;
				}
				return true;
			}
		}
	
		settings.push_back(Gc::Model::SettingsStoreRow());
		settings.back().name = name;
		settings.back().value = value;
		settings.back().validate();
	
		return false;
	}

	public: std::string addNewItem()
	{
		Gc::Model::SettingsStoreRow newRow;
		newRow.name = "";
		settings.push_back(newRow);
		return newRow.name;
	}

	public: void removeItem(std::string const& name)
	{
		for (std::list<Gc::Model::SettingsStoreRow>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
			if (iter->name == name){
				settings.erase(iter);
				break; //must break because the iterator is invalid now!
			}
		}
	}

	public: void renameItem(std::string const& old_name, std::string const& new_name)
	{
		for (std::list<Gc::Model::SettingsStoreRow>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
			if (iter->name == old_name){
				iter->name = new_name;
				break; //must break because the iterator is invalid now!
			}
		}
	}

	public: bool isActive(std::string const& name, bool checkValueToo = false)
	{
		for (std::list<Gc::Model::SettingsStoreRow>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
			if (name == iter->name)
				return iter->isActive && (!checkValueToo || iter->value != "false");
		}
		return false;
	}

	public: bool setIsActive(std::string const& name, bool value)
	{
		for (std::list<Gc::Model::SettingsStoreRow>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
			if (name == iter->name){
				if (iter->isActive != value){
					iter->isActive = value;
				}
				return true;
			}
		}
		return false;
	}

	public: bool setIsExport(std::string const& name, bool isExport)
	{
		for (std::list<Gc::Model::SettingsStoreRow>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
			if (name == iter->name){
				if (iter->hasExportPrefix != isExport){
					iter->hasExportPrefix = isExport;
				}
				return true;
			}
		}
		return false;
	}

	public: void load(FILE* source)
	{
		std::string row;
		int c;
		int step = 0; //0: name parsing, 1: value parsing
		bool inQuotes = false;
		char quoteChar;
		settings.push_back(Gc::Model::SettingsStoreRow());
		while ((c = fgetc(source)) != EOF){
			if (c == '\n'){
				settings.back().validate();
				settings.push_back(Gc::Model::SettingsStoreRow());
				inQuotes = false;
				step = 0;
			}
			else {
				settings.back().plaintext += c;
				if (c == '"' && (!inQuotes || quoteChar == c)){
					inQuotes = !inQuotes;
					quoteChar = '"';
				}
				else if (c == '\'' && (!inQuotes || quoteChar == c)){
					inQuotes = !inQuotes;
					quoteChar = '\'';
				}
				else {
					if (step == 0 && c == '=' && !inQuotes)
						step = 1;
					else if (step == 0)
						settings.back().name += c;
					else if (step == 1)
						settings.back().value += c;
				}
			}
		}
		if (settings.size() > 0)
			settings.back().validate();
	}

	public: void save(FILE* target)
	{
		for (std::list<Gc::Model::SettingsStoreRow>::iterator iter = this->begin(false); iter != this->end(); iter++){
			if (iter != this->begin(false)) {
				fputs("\n", target);
			}
			fputs((iter->getOutput()).c_str(), target);
		}
	}

	public: void clear()
	{
		this->settings.clear();
	}

};}}

#endif /* SETTINGSSTORE_H_ */
