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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#ifndef CONTENTPARSER_H_
#define CONTENTPARSER_H_
#include <map>
#include <string>

class ContentParser {
public:
	enum Exception {
		PARSING_FAILED
	};
	virtual inline ~ContentParser() {};
	virtual void parse(std::string const& sourceCode) = 0;
	virtual std::map<std::string, std::string> getOptions() const = 0;
	virtual std::string getOption(std::string const& name) const = 0;
	virtual void setOption(std::string const& name, std::string const& value) = 0;
	virtual void setOptions(std::map<std::string, std::string> const& options) = 0;
	virtual std::string buildSource() const = 0;
	virtual void buildDefaultEntry(std::string const& partition_uuid) = 0;
};

#endif /* CONTENTPARSER_H_ */
 
