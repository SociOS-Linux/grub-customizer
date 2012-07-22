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

#include "entry.h"

GrubConfRow::GrubConfRow(FILE* sourceFile)
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
GrubConfRow::GrubConfRow()
	: eof(false), is_loaded(false)
{}
GrubConfRow::operator bool(){
	return !eof && is_loaded;
}

Entry::Entry()
	: isValid(false), quote('\'')
{}

Entry::Entry(std::string name, std::string extension, std::string content, EntryType type)
	: name(name), extension(extension), content(content), isValid(true), type(type), quote('\'')
{}
Entry::Entry(FILE* sourceFile, GrubConfRow firstRow, Logger* logger, std::string* plaintextBuffer)
	: isValid(false), type(MENUENTRY), quote('\'')
{
	//int c;
	//std::string row;

	GrubConfRow row;
	bool inEntry = false;
	while ((row = firstRow) || (row = GrubConfRow(sourceFile))){
		if (inEntry && (row.text.substr(0, 10) == "menuentry " || row.text.substr(0, 8) == "submenu ")){
			this->subEntries.push_back(Entry(sourceFile, row));
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

			*this = Entry(entryName, extension);
			this->quote = quote;
			inEntry = true;
		} else if (!inEntry && row.text.substr(0, 8) == "submenu ") {
			int endOfEntryName = row.text.find('"', 10);
			if (endOfEntryName == -1)
				endOfEntryName = row.text.find('\'', 10);
			std::string entryName = row.text.substr(9, endOfEntryName-9);

			*this = Entry(entryName, "", "", SUBMENU);
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

std::list<Entry>& Entry::getSubEntries() {
	return this->subEntries;
}

Entry::operator bool() const {
	return isValid;
}


