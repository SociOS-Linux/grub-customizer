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
#include <map>

namespace Gc { namespace View { namespace Model {
template <typename TItem, typename TWrapper>
class ListItem
{
	public: std::string name;
	public: TItem* entryPtr = nullptr;
	public: TWrapper* scriptPtr = nullptr;
	public: bool is_placeholder = false;
	public: bool is_submenu = false;
	public: std::string scriptName;
	public: std::string defaultName;
	public: bool isEditable = false;
	public: bool isModified = false;
	public: std::map<std::string, std::string> options;
	public: bool isVisible = false;
	public: TItem* parentEntry = nullptr;
	public: TWrapper* parentScript = nullptr;
};}}}


