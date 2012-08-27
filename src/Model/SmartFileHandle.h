#ifndef SMARTFILEHANDLE_H_
#define SMARTFILEHANDLE_H_
#include <cstdio>
#include <string>
#include "../lib/Exception.h"

class Model_SmartFileHandle {
private:
	FILE* proc_or_file;
	bool isCmd;
public:
	enum Type {
		TYPE_FILE,
		TYPE_COMMAND
	};
	Model_SmartFileHandle();
	char getChar();
	std::string getRow();
	std::string getAll();
	void open(std::string const& cmd_or_file, std::string const& mode, Type type);
	void close();
};

#endif
