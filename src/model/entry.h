#ifndef GRUB_CUSTOMIZER_ENTRY_INCLUDED
#define GRUB_CUSTOMIZER_ENTRY_INCLUDED
#include <cstdio>
#include <string>

struct GrubConfRow {
	GrubConfRow(FILE* sourceFile);
	GrubConfRow();
	std::string text;
	bool eof;
	bool is_loaded;
	operator bool();
};

struct Entry {
	bool isValid;
	std::string name, extension, content;
	Entry();
	Entry(std::string name, std::string extension, std::string content = "");
	Entry(FILE* sourceFile, GrubConfRow firstRow = GrubConfRow());
	operator bool() const;
};

#endif
