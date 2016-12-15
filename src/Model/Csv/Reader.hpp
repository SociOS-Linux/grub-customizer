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

#ifndef CSV_READER_H_
#define CSV_READER_H_

#include <list>
#include <map>
#include <string>
#include <cstdio>
#include <stdexcept>


namespace Gc { namespace Model { namespace Csv { class Reader
{
	private: FILE* _file;

	private: std::list<std::string> _keys;

	private: std::list<std::string> _parseRow()
	{
		std::list<std::string> dataRow;
		int c = 0;
		std::string field;
		bool quotesFound = false;
		bool inString = false;
		while ((c = fgetc(this->_file)) != EOF) {
			if (c == '"') {
				if (!inString) {
					inString = true;
				} else {
					if (quotesFound) { // double quotes
						field += '"';
						quotesFound = false;
					} else {
						quotesFound = true;
					}
				}
			} else if ((c == ';' || c == '\n') && (!inString || quotesFound)) {
				quotesFound = false;
				inString = false;
				dataRow.push_back(field);
				field = "";
				if (c == '\n') {
					break;
				}
			} else if (inString) {
				field += char(c);
				quotesFound = false;
			}
		}
	
		return dataRow;
	}

	public: Reader(FILE* file) :
		_file(file)
	{}
	
	public: std::map<std::string, std::string> read() {
		if (this->_keys.size() == 0) {
			this->_keys = this->_parseRow();
		}
		std::list<std::string> values = this->_parseRow();
	
		std::map<std::string, std::string> result;
		for (std::list<std::string>::iterator keyIter = this->_keys.begin(), valueIter = values.begin(); keyIter != this->_keys.end() && valueIter != values.end(); keyIter++, valueIter++) {
			result[*keyIter] = *valueIter;
		}
	
		return result;
	}

};}}}


#endif /* CSV_READER_H_ */
