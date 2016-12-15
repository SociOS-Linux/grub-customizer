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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <string>
#include <sstream>
#include <typeinfo>

namespace Gc { namespace Common {
class Exception {
	private: std::string _message;
	private: std::string _file;
	private: int _line;

	public:	Exception(
		std::string const& message,
		std::string const& file = "",
		int line = -1
	) :
		_message(message),
		_file(file),
		_line(line)
	{}

	public:	virtual inline ~Exception(){}
	public:	virtual operator std::string() const {
		std::ostringstream out;
		out << "exception '" << typeid(*this).name()
			<< "'\n with message '" << this->_message;
		if (this->_file != "") {
			out << "'\n in " << this->_file << ":" << this->_line;
		}
		return out.str();
	}

	public:	virtual std::string getMessage() const {
		return this->_message;
	}

	public:	virtual operator bool() const {
		return this->_message != "";
	}
};

class LogicException : public Gc::Common::Exception {
	public: inline LogicException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::Exception(message, file, line) {}
};

class HandleNotOpenedException : public Gc::Common::LogicException {
	public: inline HandleNotOpenedException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class HandleNotClosedException : public Gc::Common::LogicException {
	public: inline HandleNotClosedException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class EndOfFileException : public Gc::Common::LogicException {
	public: inline EndOfFileException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class MustBeProxyException : public Gc::Common::LogicException {
	public: inline MustBeProxyException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class ItemNotFoundException : public Gc::Common::LogicException {
	public: inline ItemNotFoundException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class ParserException : public Gc::Common::LogicException {
	public: inline ParserException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class RegExNotMatchedException : public Gc::Common::LogicException {
	public: inline RegExNotMatchedException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class NoMoveTargetException : public Gc::Common::LogicException {
	public: inline NoMoveTargetException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class ParserNotFoundException : public Gc::Common::ItemNotFoundException {
	public: inline ParserNotFoundException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::ItemNotFoundException(message, file, line) {}
};

class MountpointNotFoundException : public Gc::Common::ItemNotFoundException {
	public: inline MountpointNotFoundException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::ItemNotFoundException(message, file, line) {}
};

class TechnicalException : public Gc::Common::Exception {
	public: inline TechnicalException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::Exception(message, file, line) {}
};

class AssertException : public Gc::Common::TechnicalException {
	public: inline AssertException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::TechnicalException(message, file, line) {}
};

class NullPointerException : public Gc::Common::TechnicalException {
	public: inline NullPointerException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::TechnicalException(message, file, line) {}
};

class ConfigException : public Gc::Common::TechnicalException {
	public: inline ConfigException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::TechnicalException(message, file, line) {}
};

class SystemException : public Gc::Common::TechnicalException {
	public: inline SystemException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::TechnicalException(message, file, line) {}
};

class FileReadException : public Gc::Common::SystemException {
	public: inline FileReadException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::SystemException(message, file, line) {}
};

class FileSaveException : public Gc::Common::SystemException {
	public: inline FileSaveException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::SystemException(message, file, line) {}
};

class DirectoryNotFoundException : public Gc::Common::SystemException {
	public: inline DirectoryNotFoundException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::SystemException(message, file, line) {}
};

class CmdExecException : public Gc::Common::SystemException {
	public: inline CmdExecException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::SystemException(message, file, line) {}
};

class MountException : public Gc::Common::SystemException {
	public: inline MountException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::SystemException(message, file, line) {}
};

class UMountException : public Gc::Common::SystemException {
	public: inline UMountException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::SystemException(message, file, line) {}
};

class MissingFstabException : public Gc::Common::SystemException {
	public: inline MissingFstabException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::SystemException(message, file, line) {}
};

class BadCastException : public Gc::Common::TechnicalException {
public: inline BadCastException(std::string const& message, std::string const& file = "", int line = -1)
   : Gc::Common::TechnicalException(message, file, line) {}
};

class InvalidStringFormatException : public Gc::Common::LogicException {
	public: inline InvalidStringFormatException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class InvalidFileTypeException : public Gc::Common::LogicException {
	public: inline InvalidFileTypeException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

class NotImplementedException : public Gc::Common::LogicException {
	public: inline NotImplementedException(std::string const& message, std::string const& file = "", int line = -1)
	   : Gc::Common::LogicException(message, file, line) {}
};

}}

// Template:
//class CLASSNAME : public BASECLASS {
//	public: inline  CLASSNAME(std::string const& message, std::string const& file = "", int line = -1)
//	   : BASECLASS(message, file, line) {}
//};


#endif /* EXCEPTION_H_ */
