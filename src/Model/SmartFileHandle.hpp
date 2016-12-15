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

#ifndef SMARTFILEHANDLE_H_
#define SMARTFILEHANDLE_H_
#include <cstdio>
#include <string>
#include "../Common/Exception.hpp"

namespace Gc { namespace Model { class SmartFileHandle
{
	public:	enum class Type {
		FILE,
		COMMAND,
		STRING
	};
	private: FILE* proc_or_file;
	private: Gc::Model::SmartFileHandle::Type type;
	private: std::string string; // content for TYPE_STRING

	public:	SmartFileHandle() :
		type(Gc::Model::SmartFileHandle::Type::STRING),
		proc_or_file(nullptr)
	{}

	public:	char getChar()
	{
		if (type == Gc::Model::SmartFileHandle::Type::STRING) {
			if (this->string.size()) {
				int c = (int) this->string[0];
				this->string = this->string.substr(1);
				return c;
			} else {
				throw Gc::Common::Exception::EndOfFileException("end of file", __FILE__, __LINE__);
			}
		} else {
			int c = fgetc(this->proc_or_file);
			if (c != EOF)
				return c;
			else
				throw Gc::Common::Exception::EndOfFileException("end of file", __FILE__, __LINE__);
		}
	}

	public:	std::string getRow()
	{
		std::string result;
		int c;
		try {
			while ((c = this->getChar()) != EOF && c != '\n'){
				result += c;
			}
		} catch (Gc::Common::Exception::EndOfFileException const& e) {
			if (result == "") {
				throw e;
			}
		}
		return result;
	}

	public:	std::string getAll()
	{
		std::string result;
		int c;
		try {
			while ((c = this->getChar()) != EOF){
				result += c;
			}
		} catch (Gc::Common::Exception::EndOfFileException const& e) {
			if (result == "") {
				throw e;
			}
		}
		return result;
	}

	public:	void open(std::string const& cmd_or_file, std::string const& mode, Type type)
	{
		if (this->proc_or_file || this->string != "")
			throw Gc::Common::Exception::HandleNotClosedException("handle not closed - cannot open", __FILE__, __LINE__);
	
		this->proc_or_file = NULL;
		this->string = "";
	
		switch (type) {
			case Gc::Model::SmartFileHandle::Type::STRING:
				this->string = cmd_or_file;
				break;
			case Gc::Model::SmartFileHandle::Type::COMMAND:
				this->proc_or_file = popen(cmd_or_file.c_str(), mode.c_str());
				break;
			case Gc::Model::SmartFileHandle::Type::FILE:
				this->proc_or_file = fopen(cmd_or_file.c_str(), mode.c_str());
				break;
			default:
				throw Gc::Common::Exception::LogicException("unexpected type given");
		}
	
		if (this->proc_or_file || type == Gc::Model::SmartFileHandle::Type::STRING)
			this->type = type;
		else
			throw Gc::Common::Exception::FileReadException("Cannot read the file/cmd: " + cmd_or_file, __FILE__, __LINE__);
	}

	public:	void close()
	{
		if (this->type != Gc::Model::SmartFileHandle::Type::STRING && !this->proc_or_file)
			throw Gc::Common::Exception::HandleNotOpenedException("handle not opened - cannot close", __FILE__, __LINE__);
	
		switch (type) {
			case Gc::Model::SmartFileHandle::Type::STRING:
				this->string = "";
				break;
			case Gc::Model::SmartFileHandle::Type::COMMAND:
				pclose(this->proc_or_file);
				break;
			case Gc::Model::SmartFileHandle::Type::FILE:
				fclose(this->proc_or_file);
				break;
			default:
				throw Gc::Common::Exception::LogicException("unexpected type given");
		}
	}

};}}

#endif
