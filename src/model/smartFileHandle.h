#ifndef SMARTFILEHANDLE_H_
#define SMARTFILEHANDLE_H_
#include <cstdio>
#include <string>

class SmartFileHandle {
private:
	FILE* proc_or_file;
	bool isCmd;
public:
	enum Exception {
		END_OF_FILE,
		UNABLE_TO_OPEN,
		HANDLE_NOT_CLOSED,
		HANDLE_NOT_OPENED
	};
	enum Type {
		TYPE_FILE,
		TYPE_COMMAND
	};
	SmartFileHandle();
	char getChar();
	std::string getRow();
	std::string getAll();
	void open(std::string const& cmd_or_file, std::string const& mode, Type type);
	void close();
};

#endif
