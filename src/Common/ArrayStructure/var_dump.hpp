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

#ifndef ARRAYSTRUCTUREDUMP_H_
#define ARRAYSTRUCTUREDUMP_H_
#include <string>
#include <map>
#include <list>
#include <sstream>
#include <iostream>


namespace Gc { namespace Common { namespace ArrayStructure {
// prefix "!" = use prefixes
inline void var_dump(std::map<std::string, Gc::Common::ArrayStructure::Item> const& data, std::string prefix = "", unsigned int indent = 0) {
	if (prefix[0] == '!') {
		indent = 2;
	}

	if (data.size() == 0) {
		for (unsigned int i = 0; i < indent; i++) {
			std::cout << " ";
		}
		std::cout << "(empty)" << std::endl;
	}
	for (std::map<std::string, Gc::Common::ArrayStructure::Item>::const_iterator iter = data.begin(); iter != data.end(); iter++) {
		std::string indentStr = "";
		for (unsigned int i = 0; i < indent; i++) {
			indentStr += " ";
		}
		if (prefix[0] != '!') {
			std::cout << indentStr;
		}

		std::string key = prefix + iter->first;


		if (!iter->second.isArray) {
			std::string val = iter->second.value;
			if (iter->second.isString) {
				val = "\"" + val + "\"";
			}

			std::cout << "\"" << key << "\"" << " : " << Gc::Common::Functions::str_replace("\n", "\n" + indentStr, val) << std::endl;
		} else {
			std::cout << "\"" << key << "\"" << " : Array(" << iter->second.subItems.size() << ")" << (iter->second.value != "" ? " [" + iter->second.value + "]" : "") << std::endl;
			if (iter->second.subItems.size()) {
				var_dump(iter->second.subItems, key[0] == '!' ? key + "." : "", indent + 2);
			}
		}
	}
}

}}}


#endif /* ARRAYSTRUCTUREDUMP_H_ */
