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

#ifndef CONTENT_PARSER_ABSTRACT_H_
#define CONTENT_PARSER_ABSTRACT_H_
#include <map>
#include <string>
#include "../../interface/contentParser.h"
#include "../../presenter/commonClass.h"

class ContentParserAbstract : public ContentParser, public CommonClass {
protected:
	std::map<std::string, std::string> options;
public:
	virtual inline ~ContentParserAbstract() {};
	std::map<std::string, std::string> getOptions() const;
	std::string getOption(std::string const& name) const;
	void setOption(std::string const& name, std::string const& value);
	void setOptions(std::map<std::string, std::string> const& options);
};

#endif /* CONTENT_PARSER_ABSTRACT_H_ */
