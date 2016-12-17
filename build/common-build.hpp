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

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <dirent.h>

namespace GcBuild
{

	std::string getFileContents(std::string const& file)
	{
		std::ifstream ifs(file.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

		std::ifstream::pos_type fileSize = ifs.tellg();

		if (fileSize == -1) {
			throw std::runtime_error("file not found: " + file);
		}

		ifs.seekg(0, std::ios::beg);

		std::vector<char> bytes(fileSize);
		ifs.read(&bytes[0], fileSize);

		return std::string(&bytes[0], fileSize);
	}

	std::string buildCharString(char from, char to)
	{
		std::string result;

		for (char c = from; c <= to; c++) {
			result += c;
		}

		return result;
	}

	bool isDir(std::string const& path)
	{
		DIR* dir = opendir(path.c_str());
		if (!dir) {
			return false;
		}
		closedir(dir);
		return true;
	}

	bool isFile(std::string const& path)
	{
		FILE* file = fopen(path.c_str(), "r");
		if (!file) {
			return false;
		}
		fclose(file);
		return true;
	}

	std::string stringRepeat(std::string const& str, unsigned int times)
	{
		std::string result;

		for (unsigned int i = 0; i < times; i++) {
			result += str;
		}

		return result;
	}

	std::string intToString(int number)
	{
		std::ostringstream s;
		s << number;
		return s.str();
	}

	std::string replaceStringContents(const std::string &search, const std::string &replace, std::string subject) {
		size_t pos = 0;
		while (pos < subject.length() && (pos = subject.find(search, pos)) != -1){
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
		return subject;
	}

	std::string ltrim(std::string string, std::string const& chars = " \t\n\r") {
		int first = string.find_first_not_of(chars);
		if (first != -1) {
			return string.substr(first);
		} else {
			return "";
		}
	}

	std::string rtrim(std::string string, std::string const& chars = " \t\n\r") {
		string = std::string(string.rbegin(), string.rend());
		string = ltrim(string, chars);
		string = std::string(string.rbegin(), string.rend());
		return string;
	}

	std::string trim(std::string string, std::string const& chars = " \t\n\r") {
		return rtrim(ltrim(string, chars));
	}
}
