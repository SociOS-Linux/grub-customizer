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

#ifndef GRUB_CUSTOMIZER_ENTRY_INCLUDED
#define GRUB_CUSTOMIZER_ENTRY_INCLUDED
#include <cstdio>
#include <string>
#include <list>
#include <memory>

namespace Gc { namespace Model { namespace ListCfg { class Entry :
	public Gc::Model::Logger::Trait::LoggerAware,
	public Gc::Common::Type::Entry
{
	public: enum EntryType {
		MENUENTRY,
		SUBMENU,
		SCRIPT_ROOT,
		PLAINTEXT
	};

	public: EntryType type;
	public: bool isValid, isModified;
	public: std::string name, extension, content;
	public: char quote;
	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Entry>> subEntries;

	public: Entry()
		: isValid(false), isModified(false), quote('\''), type(MENUENTRY)
	{}

	public: Entry(std::string name, std::string extension, std::string content = "", EntryType type = MENUENTRY)
		: name(name), extension(extension), content(content), isValid(true), type(type), isModified(false), quote('\'')
	{}
	
	public: Entry(FILE* sourceFile, Gc::Model::ListCfg::EntryRow firstRow = Gc::Model::ListCfg::EntryRow(), std::shared_ptr<Gc::Model::Logger::GenericLogger> logger = nullptr, std::string* plaintextBuffer = NULL)
		: isValid(false), type(MENUENTRY), quote('\''), isModified(false)
	{
		if (logger) {
			this->setLogger(logger);
		}
		Gc::Model::ListCfg::EntryRow row;
		while ((row = firstRow) || (row = Gc::Model::ListCfg::EntryRow(sourceFile))){
			std::string rowText = Gc::Common::Functions::ltrim(row.text);
	
			if (rowText.substr(0, 10) == "menuentry "){
				this->readMenuEntry(sourceFile, row);
				break;
			} else if (rowText.substr(0, 8) == "submenu ") {
				this->readSubmenu(sourceFile, row);
				break;
			} else {
				if (plaintextBuffer) {
					*plaintextBuffer += row.text + "\r\n";
				}
			}
			firstRow.eof = true; //disable firstRow to read the following config from file
		}
	}
	
	private: void readSubmenu(FILE* sourceFile, Gc::Model::ListCfg::EntryRow firstRow)
	{
		std::string rowText = Gc::Common::Functions::ltrim(firstRow.text);
		int endOfEntryName = rowText.find('"', 10);
		if (endOfEntryName == -1)
			endOfEntryName = rowText.find('\'', 10);
		std::string entryName = rowText.substr(9, endOfEntryName-9);
	
		*this = Gc::Model::ListCfg::Entry(entryName, "", "", SUBMENU);
		if (this->logger) {
			this->setLogger(this->logger);
		}
		Gc::Model::ListCfg::EntryRow row;
		while ((row = Gc::Model::ListCfg::EntryRow(sourceFile))) {
			std::string rowText = Gc::Common::Functions::ltrim(row.text);
	
			if (rowText.substr(0, 10) == "menuentry " || rowText.substr(0, 8) == "submenu "){
				this->subEntries.push_back(std::make_shared<Gc::Model::ListCfg::Entry>(sourceFile, row));
			} else if (Gc::Common::Functions::trim(rowText) == "}") {
				this->isValid = true;
				break; //read only one submenu
			}
		}
	}

	private: void readMenuEntry(FILE* sourceFile, Gc::Model::ListCfg::EntryRow firstRow)
	{
		std::string rowText = Gc::Common::Functions::ltrim(firstRow.text);
		char quote = '"';
		int endOfEntryName = rowText.find('"', 12);
		if (endOfEntryName == -1) {
			endOfEntryName = rowText.find('\'', 12);
			quote = '\'';
		}
		std::string entryName = rowText.substr(11, endOfEntryName-11);
	
		std::string extension = rowText.substr(endOfEntryName+1, rowText.length()-(endOfEntryName+1)-1);
	
		*this = Gc::Model::ListCfg::Entry(entryName, extension);
		if (this->logger) {
			this->setLogger(this->logger);
		}
		this->quote = quote;
	
		// encapsulated menuentries must be ignored. This variable counts the encapsulation level.
		// We're starting inside of a menuentry!
		int depth = 1;
	
	
		Gc::Model::ListCfg::EntryRow row;
		while ((row = Gc::Model::ListCfg::EntryRow(sourceFile))){
			std::string rowText = Gc::Common::Functions::ltrim(row.text);
	
			if (Gc::Common::Functions::trim(rowText) == "}" && --depth == 0) {
				this->isValid = true;
				break; //read only one menuentry
			} else {
				if (rowText.substr(0, 10) == "menuentry ") {
					depth++;
				}
				this->content += row.text+"\n";
			}
		}
	}

	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Entry>>& getSubEntries()
	{
		return this->subEntries;
	}

	public: operator bool() const
	{
		return isValid;
	}

	public: operator Gc::Common::ArrayStructure::Container() const
	{
		Gc::Common::ArrayStructure::Container result;
		result["type"] = this->type;
		result["isValid"] = this->isValid;
		result["isModified"] = this->isModified;
		result["name"] = this->name;
		result["extension"] = this->extension;
		result["content"] = this->content;
		result["quote"] = this->quote;
		result["subEntries"].isArray = true;
		result["rulepointer"] = this;
		int i = 0;
		for (auto entry : this->subEntries) {
			result["subEntries"][i] = Gc::Common::ArrayStructure::Container(*entry);
			i++;
		}

		return result;
	}
};}}}

#endif
