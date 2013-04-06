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

#ifndef THEMEMANAGER_H_
#define THEMEMANAGER_H_

#include <list>
#include "Theme.h"
#include "Env.h"
#include "../lib/Exception.h"

class Model_ThemeManager {
	Model_Env& env;
public:
	std::list<Model_Theme> themes;
	std::list<Model_Theme> removedThemes;
	Model_ThemeManager(Model_Env& env);
	void load();
	Model_Theme& getTheme(std::string const& name);
	bool themeExists(std::string const& name);
	std::string addThemePackage(std::string const& fileName);
	void removeTheme(Model_Theme const& theme);
	void save();
};


#endif /* THEMEMANAGER_H_ */
