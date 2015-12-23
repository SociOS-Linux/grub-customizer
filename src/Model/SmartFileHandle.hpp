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
#include "../lib/Exception.hpp"

class Model_SmartFileHandle {
	public: enum Type {
		TYPE_FILE,
		TYPE_COMMAND,
		TYPE_STRING
	};

	private: std::shared_ptr<InputStream> stream;
	private: Model_SmartFileHandle::Type type;
	private: std::string string; // content for TYPE_STRING

	public: Model_SmartFileHandle() : type(TYPE_STRING), stream(nullptr)
	{}

	public: char getChar() {
		if (type == TYPE_STRING) {
			if (this->string.size()) {
				int c = (int) this->string[0];
				this->string = this->string.substr(1);
				return c;
			} else {
				throw EndOfFileException("end of file", __FILE__, __LINE__);
			}
		} else {
			try {
				return this->stream->read();
			} catch (std::runtime_error const& e) {
				throw EndOfFileException("end of file", __FILE__, __LINE__);
			}
		}
	}

	public: std::string getRow() {
		std::string result;
		char c;
		try {
			while ((c = this->getChar()) != '\n'){
				result += c;
			}
		} catch (EndOfFileException const& e) {
			if (result == "") {
				throw e;
			}
		}
		return result;
	}

	public: std::string getAll() {
		std::string result;
		char c;
		try {
			while (true){
				result += this->getChar();
			}
		} catch (EndOfFileException const& e) {
			if (result == "") {
				throw e;
			}
		}
		return result;
	}

	public: void open(std::string const& cmd_or_file, Type type) {
		if (this->stream || this->string != "")
			throw HandleNotClosedException("handle not closed - cannot open", __FILE__, __LINE__);
	
		this->stream = nullptr;
		this->string = "";
	
		switch (type) {
			case TYPE_STRING: {
				this->string = cmd_or_file;
				break;
			} case TYPE_COMMAND: {
				auto pipe = Pipe::create();
				this->stream = pipe->getReader();
				Process::create("sh")
					->setArguments({"-c", cmd_or_file})
					->setStdOut(pipe->getWriter())
					->run();
				break;
			} case TYPE_FILE: {
				try {
					this->stream = File::openInputFile(cmd_or_file);
				} catch (std::runtime_error const& e) {
					this->stream = nullptr;
				}
				break;
			} default: {
				throw LogicException("unexpected type given");
			}
		}
	
		if (this->stream || type == TYPE_STRING)
			this->type = type;
		else
			throw FileReadException("Cannot read the file/cmd: " + cmd_or_file, __FILE__, __LINE__);
	}

	public: void close() {
		if (this->type != TYPE_STRING && !this->stream)
			throw HandleNotOpenedException("handle not opened - cannot close", __FILE__, __LINE__);
	
		switch (type) {
			case TYPE_STRING:
				this->string = "";
				break;
			case TYPE_COMMAND:
			case TYPE_FILE:
				this->stream->close();
				break;
			default:
				throw LogicException("unexpected type given");
		}
	}

};

#endif
