#include "confparser.h"
Entry::Entry(std::string name, std::string extension, std::string content, bool disabled)
	: name(name), outputName(name), extension(extension), content(content), isValid(true), disabled(disabled)
{}
Entry::Entry() : isValid(false) {}
Entry::operator bool () const {
	return isValid;
}

EntryList::EntryList(FILE* sourceFile): other_entries_pos(-1), other_entries_visible(false) {
	Entry grubEntry;
	while (grubEntry = readGrubEntry(sourceFile))
		this->push_back(grubEntry);
}

EntryList::EntryList(): other_entries_pos(-1), other_entries_visible(false) {}

std::list<Entry>::iterator EntryList::getEntryIterByName(std::string const& name) {
	for (std::list<Entry>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->name == name){
			return iter;
		}
	}
	return this->end();
}

GrubConfRow readGrubConfRow(FILE* sourceFile){
	GrubConfRow result;
	result.eof = true; //will be set to false on the first loop run
	int c;
	while ((c = fgetc(sourceFile)) != EOF){
		result.eof = false;
		if (c != '\n'){
			result.text += char(c);
		}
		else {
			break;
		}
	}
	return result;
}
GrubConfRow::operator bool(){
	return !eof;
}
Entry readGrubEntry(FILE* sourceFile, GrubConfRow firstRow){
	//int c;
	//std::string row;
	
	GrubConfRow row;
	bool inEntry = false;
	Entry result;
	while ((row = firstRow) || (row = readGrubConfRow(sourceFile))){
		if (inEntry && row.text != "}"){
			result.content += row.text+"\n";
		}
		else if (inEntry && row.text == "}"){
			//std::cout << "end of entry!" << std::endl;
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
		
			result = Entry(entryName, extension);
			inEntry = true;
		}
		firstRow.eof = true; //disable firstRow to read the following config from file
	}
	return result;
}

