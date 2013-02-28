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

#include "Theme.h"

Model_Theme::Model_Theme(std::string const& directory, std::string const& name)
	: directory(directory), name(name)
{
	this->load(directory);
}

void Model_Theme::load(std::string const& directory) {
	DIR* dir = opendir(directory.c_str());
	if (dir) {
		struct dirent *entry;
		struct stat fileProperties;
		while ((entry = readdir(dir))) {
			if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
				continue;
			}
			std::string currentFileName = directory + "/" + entry->d_name;
			stat(currentFileName.c_str(), &fileProperties);
			if (S_ISDIR(fileProperties.st_mode)) {
				this->load(currentFileName);
			} else {
				this->files.push_back(Model_ThemeFile(this->extractLocalPath(currentFileName)));
			}
		}
		closedir(dir);
		this->files.sort(&Model_ThemeFile::compareLocalPath);
	} else {
		throw FileReadException("cannot read the theme directory: " + this->directory);
	}
}

std::string Model_Theme::loadFileContent(std::string localFileName) {
	std::string data;
	FILE* file = fopen((this->directory + "/" + localFileName).c_str(), "r");
	if (file) {
		data.reserve(10240);
		int c;
		while ((c = fgetc(file)) != EOF) {
			data += char(c);
		}
		fclose(file);
	} else {
		throw FileReadException("cannot read file: " + localFileName, __FILE__, __LINE__);
	}
	return data;
}

std::string Model_Theme::getFullFileName(std::string localFileName) {
	return this->directory + "/" + localFileName;
}


std::string Model_Theme::extractLocalPath(std::string fullPath) {
	return fullPath.substr(this->directory.size() + 1);
}
