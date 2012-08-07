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

#ifndef CONTENT_PARSER_LINUX_H_
#define CONTENT_PARSER_LINUX_H_
#include "../regex.h"
#include "../../model/grubDeviceMap.h"
#include "Abstract.h"

class ContentParserLinux : public ContentParserAbstract {
	static const char* _regex;
	GrubDeviceMap& deviceMap;
	std::string sourceCode;
public:
	ContentParserLinux(GrubDeviceMap& deviceMap);
	void parse(std::string const& sourceCode);
	std::string buildSource() const;
	void buildDefaultEntry(std::string const& partition_uuid);
};

#endif /* CONTENT_PARSER_LINUX_H_ */
