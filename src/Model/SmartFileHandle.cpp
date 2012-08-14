#include "SmartFileHandle.h"

Model_SmartFileHandle::Model_SmartFileHandle()
	: isCmd(false), proc_or_file(NULL)
{
}

char Model_SmartFileHandle::getChar() {
	int c = fgetc(this->proc_or_file);
	if (c != EOF)
		return c;
	else
		throw END_OF_FILE;
}
std::string Model_SmartFileHandle::getRow() {
	std::string result;
	int c;
	while ((c = fgetc(this->proc_or_file)) != EOF && c != '\n'){
		result += c;
	}
	if (result == "" && c == EOF)
		throw END_OF_FILE;
	return result;
}
std::string Model_SmartFileHandle::getAll() {
	std::string result;
	int c;
	while ((c = fgetc(this->proc_or_file)) != EOF){
		result += c;
	}
	if (result == "" && c == EOF)
		throw END_OF_FILE;
	return result;
}


void Model_SmartFileHandle::open(std::string const& cmd_or_file, std::string const& mode, Type type) {
	if (this->proc_or_file)
		throw HANDLE_NOT_CLOSED;

	if (type == TYPE_COMMAND)
		this->proc_or_file = popen(cmd_or_file.c_str(), mode.c_str());
	else if (type == TYPE_FILE)
		this->proc_or_file = fopen(cmd_or_file.c_str(), mode.c_str());

	if (this->proc_or_file)
		this->isCmd = type == TYPE_COMMAND;
	else
		throw UNABLE_TO_OPEN;
}
void Model_SmartFileHandle::close() {
	if (!this->proc_or_file)
		throw HANDLE_NOT_OPENED;

	if (isCmd)
		pclose(this->proc_or_file);
	else
		fclose(this->proc_or_file);
}
