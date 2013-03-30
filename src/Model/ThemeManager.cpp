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

#include "ThemeManager.h"

Model_ThemeManager::Model_ThemeManager(Model_Env& env)
	: env(env)
{}

void Model_ThemeManager::load() {
	std::string path = this->env.output_config_dir + "/" + "themes";

	DIR* dir = opendir(path.c_str());
	if (dir) {
		struct dirent *entry;
		struct stat fileProperties;
		while ((entry = readdir(dir))) {
			if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
				continue;
			}
			std::string currentFileName = path + "/" + entry->d_name;
			stat(currentFileName.c_str(), &fileProperties);
			this->themes.push_back(Model_Theme(currentFileName, "", entry->d_name));
		}
		closedir(dir);
	} else {
		throw FileReadException("cannot read the theme directory: " + path);
	}
}

Model_Theme& Model_ThemeManager::getTheme(std::string const& name) {
	for (std::list<Model_Theme>::iterator themeIter = this->themes.begin(); themeIter != this->themes.end(); themeIter++) {
		if (themeIter->name == name) {
			return *themeIter;
		}
	}
	throw ItemNotFoundException("getTheme: Theme not found: " + name, __FILE__, __LINE__);
}

std::string Model_ThemeManager::addThemePackage(std::string const& fileName) {
	this->themes.push_back(Model_Theme("", fileName, fileName));
	return fileName;
}
