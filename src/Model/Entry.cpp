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

#include "Entry.h"

Model_Entry_Row::Model_Entry_Row(FILE* sourceFile)
	: eof(false), is_loaded(true)
{
	this->eof = true; //will be set to false on the first loop run
	int c;
	while ((c = fgetc(sourceFile)) != EOF){
		this->eof = false;
		if (c != '\n'){
			this->text += char(c);
		}
		else {
			break;
		}
	}
}
Model_Entry_Row::Model_Entry_Row()
	: eof(false), is_loaded(false)
{}
Model_Entry_Row::operator bool(){
	return !eof && is_loaded;
}

Model_Entry::Model_Entry()
	: isValid(false), isModified(false), quote('\''), type(MENUENTRY)
{}

Model_Entry::Model_Entry(std::string name, std::string extension, std::string content, EntryType type)
	: name(name), extension(extension), content(content), isValid(true), type(type), isModified(false), quote('\'')
{}
Model_Entry::Model_Entry(FILE* sourceFile, Model_Entry_Row firstRow, Logger* logger, std::string* plaintextBuffer)
	: isValid(false), type(MENUENTRY), quote('\''), isModified(false)
{
	//int c;
	//std::string row;

	Model_Entry_Row row;
	bool inEntry = false;
	while ((row = firstRow) || (row = Model_Entry_Row(sourceFile))){
		if (inEntry && (row.text.substr(0, 10) == "menuentry " || row.text.substr(0, 8) == "submenu ")){
			this->subEntries.push_back(Model_Entry(sourceFile, row));
		} else if (inEntry && row.text != "}") {
			this->content += row.text+"\n";
		} else if (inEntry && row.text == "}") {
			//std::cout << "end of entry!" << std::endl;
			isValid = true;
			inEntry = false;
			break; //nur einen Eintrag lesen!
		} else if (!inEntry && row.text.substr(0, 10) == "menuentry "){
			char quote = '"';
			int endOfEntryName = row.text.find('"', 12);
			if (endOfEntryName == -1) {
				endOfEntryName = row.text.find('\'', 12);
				quote = '\'';
			}
			std::string entryName = row.text.substr(11, endOfEntryName-11);

			std::string extension = row.text.substr(endOfEntryName+1, row.text.length()-(endOfEntryName+1)-1);

			*this = Model_Entry(entryName, extension);
			this->quote = quote;
			inEntry = true;
		} else if (!inEntry && row.text.substr(0, 8) == "submenu ") {
			int endOfEntryName = row.text.find('"', 10);
			if (endOfEntryName == -1)
				endOfEntryName = row.text.find('\'', 10);
			std::string entryName = row.text.substr(9, endOfEntryName-9);

			*this = Model_Entry(entryName, "", "", SUBMENU);
			if (logger) {
				this->setLogger(*logger);
			}
			inEntry = true;
		} else {
			if (plaintextBuffer) {
				*plaintextBuffer += row.text + "\r\n";
			}
		}
		firstRow.eof = true; //disable firstRow to read the following config from file
	}
}

std::list<Model_Entry>& Model_Entry::getSubEntries() {
	return this->subEntries;
}

Model_Entry::operator bool() const {
	return isValid;
}

Model_Entry::operator ArrayStructure() const {
	ArrayStructure result;
	result["type"] = this->type;
	result["isValid"] = this->isValid;
	result["isModified"] = this->isModified;
	result["name"] = this->name;
	result["extension"] = this->extension;
	result["content"] = this->content;
	result["quote"] = this->quote;
	result["subEntries"].isArray = true;
	int i = 0;
	for (std::list<Model_Entry>::const_iterator iter = this->subEntries.begin(); iter != this->subEntries.end(); iter++) {
		result["subEntries"][i] = ArrayStructure(*iter);
		i++;
	}

	return result;
}
