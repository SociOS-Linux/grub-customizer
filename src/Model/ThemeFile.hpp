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

#ifndef THEMEFILE_H_
#define THEMEFILE_H_
#include <string>

namespace Gc { namespace Model { class ThemeFile
{
	public: std::string localFileName, newLocalFileName; // path inside of the theme directory
	public: bool contentLoaded; // say whether the content is loaded (text only)
	public: std::string content; // loaded content (text only)
	public: bool isAddedByUser;
	public: std::string externalSource;

	public: ThemeFile(std::string localFileName, bool isAddedByUser = false) :
		localFileName(localFileName),
		contentLoaded(false),
		newLocalFileName(localFileName),
		isAddedByUser(isAddedByUser)
	{}

	public: static bool compareLocalPath(Gc::Model::ThemeFile const& a, Gc::Model::ThemeFile const& b)
	{
		return a.newLocalFileName < b.newLocalFileName;
	}
};}}


#endif /* THEMEFILE_H_ */
