#include "mountTable.h"

bool MTabEntry::isValid() const {
	return device != "" && mountpoint != "" && fileSystem != "" && options != "" && dump != "" && pass != "";
}
MTabEntry::operator bool() const {
	return this->isValid();
}

MountTable::MountTable(FILE* source){
	loadData(source);
}

MountTable::MountTable(){}

void MountTable::loadData(FILE* source){
	int c;
	int rowEntryPos = 0;
	bool isComment = false;
	bool isBeginOfRow = true;
	char previousChar = 0;
	this->push_back(MTabEntry());
	while ((c = fgetc(source)) != EOF){
		if (isBeginOfRow && c == '#')
			isComment = true;
		else if (c == '\n'){
			if (!this->back().isValid())
				this->pop_back();
			this->push_back(MTabEntry());
			rowEntryPos = 0;
			isBeginOfRow = true;
			isComment = false;
		}
		else if (!isComment) {
			if (c == ' ' || c == '\t'){
				if (previousChar != ' ' && previousChar != '\t')
					rowEntryPos++;
				isBeginOfRow = false;
			}
			else {
				switch (rowEntryPos){
					case 0:	this->back().device += char(c); break;
					case 1: this->back().mountpoint += char(c); break;
					case 2: this->back().fileSystem += char(c); break;
					case 3: this->back().options += char(c); break;
					case 4: this->back().dump += char(c); break;
					case 5: this->back().pass += char(c); break;
				}
				isBeginOfRow = false;
			}
		}
		previousChar = c;
	}
	if (!this->back().isValid())
		this->pop_back();
	
	loaded = true;
}

bool MountTable::isLoaded() const {
	return loaded;
}

MountTable::operator bool() const {
	return isLoaded();
}

MTabEntry MountTable::getEntryByMountpoint(std::string const& mountPoint){
	for (std::list<MTabEntry>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->mountpoint == mountPoint)
			return *iter;
	}
	return MTabEntry();
}

