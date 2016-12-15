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
 
#ifndef CONTENTPARSER_H_
#define CONTENTPARSER_H_
#include <map>
#include <string>

namespace Gc { namespace Model { namespace ContentParser { class GenericParser
{
	public: virtual inline ~GenericParser() {};
	public: virtual void parse(std::string const& sourceCode) = 0;
	public: virtual std::map<std::string, std::string> getOptions() const = 0;
	public: virtual std::string getOption(std::string const& name) const = 0;
	public: virtual bool hasOption(std::string const& name) const = 0;
	public: virtual void setOption(std::string const& name, std::string const& value) = 0;
	public: virtual void setOptions(std::map<std::string, std::string> const& options) = 0;
	public: virtual std::string buildSource() const = 0;
	public: virtual void buildDefaultEntry() = 0;
	public: virtual std::list<std::string> getErrors() = 0;
};}}}

#endif /* CONTENTPARSER_H_ */
 
