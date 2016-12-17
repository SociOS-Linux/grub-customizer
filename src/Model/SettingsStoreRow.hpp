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


namespace Gc { namespace Model { class SettingsStoreRow
{
	public: SettingsStoreRow() :
		isActive(true),
		hasExportPrefix(false),
		isSetting(true)
	{}

	public: std::string name, value, comment, plaintext;

	public: bool hasExportPrefix, isActive, isSetting;

	public: void validate()
	{
		isActive = false;
		hasExportPrefix = false;
		isSetting = false;
	
		// trim name and value
		name = name.substr(name.find_first_not_of(' ') == -1 ? 0 : name.find_first_not_of(' ')); // ltrim
		name = name.substr(0, name.find_last_not_of(' ') + 1); // rtrim
		value = value.substr(value.find_first_not_of(' ') == -1 ? 0 : value.find_first_not_of(' ')); // ltrim
		value = value.substr(0, value.find_last_not_of(' ') + 1); // rtrim
	
		if (name != "" && value != "" && (name.length() < 2 || name.substr(0,2) != "# ")){
			isSetting = true;
			if (name[0] != '#')
				isActive = true;
			else
				name = name.substr(1);
	
			if (name.length() > 7 && name.substr(0,7) == "export "){
				hasExportPrefix = true;
				int pos = name.find_first_not_of(' ', 7);
				name = name.substr(pos != -1 ? pos : 7); // try to use trimmed value, but use pos 7 if there's an error
			}
		}
		else {
			name = "";
			value = "";
		}
	}

	public: std::string getOutput()
	{
		if (isSetting) {
			if (name != "") {
				return (isActive ? "" : "#")+std::string(hasExportPrefix ? "export " : "")+name+"=\""+value+"\""+(comment != "" ? " #"+comment : "");
			} else {
				return "#UNNAMED_OPTION=\""+value+"\""; //unnamed options would destroy the grub confuguration
			}
		} else {
			return plaintext;
		}
	}

};}}

