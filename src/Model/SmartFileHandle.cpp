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
		throw EndOfFileException("end of file", __FILE__, __LINE__);
}
std::string Model_SmartFileHandle::getRow() {
	std::string result;
	int c;
	while ((c = fgetc(this->proc_or_file)) != EOF && c != '\n'){
		result += c;
	}
	if (result == "" && c == EOF)
		throw EndOfFileException("end of file", __FILE__, __LINE__);
	return result;
}
std::string Model_SmartFileHandle::getAll() {
	std::string result;
	int c;
	while ((c = fgetc(this->proc_or_file)) != EOF){
		result += c;
	}
	if (result == "" && c == EOF)
		throw EndOfFileException("end of file", __FILE__, __LINE__);
	return result;
}


void Model_SmartFileHandle::open(std::string const& cmd_or_file, std::string const& mode, Type type) {
	if (this->proc_or_file)
		throw HandleNotClosedException("handle not closed - cannot open", __FILE__, __LINE__);

	if (type == TYPE_COMMAND)
		this->proc_or_file = popen(cmd_or_file.c_str(), mode.c_str());
	else if (type == TYPE_FILE)
		this->proc_or_file = fopen(cmd_or_file.c_str(), mode.c_str());

	if (this->proc_or_file)
		this->isCmd = type == TYPE_COMMAND;
	else
		throw FileReadException("Cannot rad the file", __FILE__, __LINE__);
}
void Model_SmartFileHandle::close() {
	if (!this->proc_or_file)
		throw HandleNotOpenedException("handle not opened - cannot close", __FILE__, __LINE__);

	if (isCmd)
		pclose(this->proc_or_file);
	else
		fclose(this->proc_or_file);
}
