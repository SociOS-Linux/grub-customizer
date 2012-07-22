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
	: isValid(false)
{}

Entry::Entry(std::string name, std::string extension, std::string content)
	: name(name), extension(extension), content(content), isValid(true)
{}
Entry::Entry(FILE* sourceFile, GrubConfRow firstRow)
	: isValid(false)
{
	//int c;
	//std::string row;

	GrubConfRow row;
	bool inEntry = false;
	while ((row = firstRow) || (row = GrubConfRow(sourceFile))){
		if (inEntry && row.text != "}"){
			this->content += row.text+"\n";
		}
		else if (inEntry && row.text == "}"){
			//std::cout << "end of entry!" << std::endl;
			isValid = true;
			inEntry = false;
			break; //nur einen Eintrag lesen!
		}
		else if (!inEntry && row.text.substr(0, 10) == "menuentry "){
			int endOfEntryName = row.text.find('"', 12);
			if (endOfEntryName == -1)
				endOfEntryName = row.text.find('\'', 12);
			std::string entryName = row.text.substr(11, endOfEntryName-11);
			//std::cout << "entry: " << entryName << std::endl;
			//data[activeScript].push_back(BootEntry(entryName, ""));
	
			std::string extension = row.text.substr(endOfEntryName+1, row.text.length()-(endOfEntryName+1)-1);
	
			*this = Entry(entryName, extension);
			inEntry = true;
		}
		firstRow.eof = true; //disable firstRow to read the following config from file
	}
}

Entry::operator bool() const {
	return isValid;
}

