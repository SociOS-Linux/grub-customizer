#ifndef GRUB_CONFPARSER_INCLUDED
#define GRUB_CONFPARSER_INCLUDED
#include <string>
#include <cstdio>
#include <list>
#include <iostream>

struct Entry {
	std::string name, outputName, extension, content;
	bool isValid;
	bool disabled;
	Entry(std::string name, std::string extension = "", std::string content = "", bool disabled = false);
	Entry();
	operator bool () const;
};

struct EntryList : public std::list<Entry> {
	EntryList(FILE* sourceFile);
	EntryList();
	std::list<Entry>::iterator getEntryIterByName(std::string const& name);
	int other_entries_pos;
	bool other_entries_visible;
};

struct GrubConfRow {
	std::string text;
	bool eof;
	operator bool();
};
GrubConfRow readGrubConfRow(FILE* sourceFile);
Entry readGrubEntry(FILE* source, GrubConfRow firstRow = GrubConfRow());

#endif
