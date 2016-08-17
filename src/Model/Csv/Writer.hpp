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

#ifndef CSV_WRITER_H_
#define CSV_WRITER_H_

#include <list>
#include <map>
#include <string>
#include <cstdio>
#include <stdexcept>

#include "../../Common/Helper.hpp"

namespace Gc { namespace Model { namespace Csv { class Writer
{
	private: FILE* _file;

	private: std::list<std::string> _keys;

	private: void _writeValue(std::string const& value)
	{
		fputs(("\"" + Helper::str_replace("\"", "\"\"", value) + "\"").c_str(), this->_file);
	}

	public: Writer(FILE* file) : _file(file) {}

	public: void write(std::map<std::string, std::string> const& data) {
		if (this->_keys.size() == 0) {
			for (std::map<std::string, std::string>::const_iterator cellIter = data.begin(); cellIter != data.end(); cellIter++) {
				this->_keys.push_back(cellIter->first);
	
				if (cellIter != data.begin()) {
					fputc(';', this->_file);
				}
				this->_writeValue(cellIter->first);
			}
			fputc('\n', this->_file);
		}
	
		for (std::list<std::string>::iterator keyIter = this->_keys.begin(); keyIter != this->_keys.end(); keyIter++) {
			std::string value;
			try {
				value = data.at(*keyIter);
			} catch (std::out_of_range const& e) {
				value = "";
			}
			if (keyIter != this->_keys.begin()) {
				fputc(';', this->_file);
			}
			this->_writeValue(value);
		}
		fputc('\n', this->_file);
	}

};}}}


#endif /* CSV_WRITER_H_ */
