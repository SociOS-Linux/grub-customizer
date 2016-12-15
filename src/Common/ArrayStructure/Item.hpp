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

#ifndef ARRAYSTRUCTUREITEM_H_
#define ARRAYSTRUCTUREITEM_H_
#include <string>
#include <map>
#include <list>
#include <sstream>
#include <iostream>

namespace Gc { namespace Common { namespace ArrayStructure { class Item
{
	public: Item(std::string const& value = "") :
		isArray(false),
		value(value),
		isString(true)
	{}
	
	public: bool isArray;
	public: bool isString;
	public: std::string value;
	public: std::map<std::string, Gc::Common::ArrayStructure::Item> subItems;

	public: std::string const& operator=(std::string const& value) {
		this->isArray = false;
		this->isString = true;
		return this->value = value;
	}

	public: std::string const& operator=(double const& value) {
		std::ostringstream str;
		str << value;
		this->isArray = false;
		this->isString = false;
		this->value = str.str();
		return this->value;
	}

	public: std::string const& operator=(bool const& value) {
		this->isArray = false;
		this->isString = false;
		this->value = value ? "true" : "false";
		return this->value;
	}

	public: std::string const& operator=(void const* value) {
		this->isArray = false;
		this->isString = false;
		if (value == NULL) {
			this->value = "NULL";
		} else {
			std::ostringstream str;
			str << value;
			this->value = str.str();
		}
		return this->value;
	}

	public: std::string const& operator=(char const* value) {
		this->isArray = false;
		this->isString = true;
		return this->value = value;
	}

	public: std::string const& operator=(int value) {
		std::ostringstream str;
		str << value;
		this->isArray = false;
		this->isString = false;
		this->value = str.str();
		return this->value;
	}

	public: std::map<std::string, Gc::Common::ArrayStructure::Item> const& operator=(std::map<std::string, Gc::Common::ArrayStructure::Item> const& value) {
		this->isArray = true;
		this->isString = false;
		this->subItems = value;
		return value;
	}

	public: Gc::Common::ArrayStructure::Item& operator[] (std::string const& key) {
		this->isArray = true;
		this->isString = false;
		return this->subItems[key];
	}

	public: Gc::Common::ArrayStructure::Item& operator[] (int const& key) {
		std::ostringstream str;
		str << key;
		Gc::Common::ArrayStructure::Item newItem;
		this->isArray = true;
		this->isString = false;
		return this->subItems[str.str()];
	}
};}}}



#endif /* ARRAYSTRUCTUREITEM_H_ */
