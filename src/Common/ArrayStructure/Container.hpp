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

#ifndef ARRAYSTRUCTURE_H_
#define ARRAYSTRUCTURE_H_
#include <string>
#include <map>
#include <list>
#include <sstream>
#include <iostream>

#include "Item.hpp"


/**
 * used to dump objects implementing a cast to ArrayStructure
 *
 * Example of usage:
 * 	ArrayStructure test;
 *  test["x"] = "Gc::Model::ListCfg::Proxy";
 *  test["x"]["y"];
 *  test["x"]["z"] = (Foo*)NULL;
 *  test["x"]["a"] = 10.4;
 *  test["x"]["bool1"] = true;
 *  test["x"]["bool2"] = false;
 *  test["a"] = "eins";
 *  test["null-test"] = (Foo*)NULL;

 *  ArrayStructure subStructure;
 *  subStructure["BLUBB"] = true;
 *  test["SUB"] = subStructure;

 *  var_dump(test)

 */
namespace Gc { namespace Common { namespace ArrayStructure { class Container :
	public std::map<std::string, Gc::Common::ArrayStructure::Item>
{
	public: Container() {}

	public: Container(std::list<std::string> const& source) {
		int i = 0;
		for (std::list<std::string>::const_iterator iter = source.begin(); iter != source.end(); iter++) {
			std::ostringstream str;
			str << i;
			Gc::Common::ArrayStructure::Item newItem;
			newItem.value = *iter;
			(*this)[str.str()] = newItem;
			i++;
		}
	}

	public: Container(std::map<std::string, std::string> const& source) {
		for (std::map<std::string, std::string>::const_iterator iter = source.begin(); iter != source.end(); iter++) {
			(*this)[iter->first] = iter->second;
		}
	}

};}}}


#endif /* ARRAYSTRUCTURE_H_ */
