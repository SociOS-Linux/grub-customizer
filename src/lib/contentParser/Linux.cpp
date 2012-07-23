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

#include "Linux.h"

const char* ContentParserLinux::_regex = "\
[ \t]*set root='\\(hd([0-9]+)[^0-9]+([0-9]+)\\)'\\n\
[ \t]*search[ \t]+--no-floppy[ \t]+--fs-uuid[ \t]+--set(?:=root)? ([-0-9a-fA-F]+)\\n\
([ \t]*echo[ \t]+.*\n)?\
[ \t]*linux[ \t]+([^ \t]+)[ \t]+root=UUID=([-0-9a-fA-F]+) .*\\n\
([ \t]*echo[ \t]+.*\n)?\
[ \t]*initrd[ \t]+([^ \\n]+)[ \\n\t]*$\
";

void ContentParserLinux::parse(std::string const& sourceCode) {
	try {
		std::vector<std::string> result = Regex::match(ContentParserLinux::_regex, sourceCode);

		this->options["partition_uuid"] = result[6];
		this->options["linux_image"] = result[5];
		this->options["initramfs"] = result[8];
	} catch (Regex::Exception e) {
		throw ContentParser::PARSING_FAILED;
	}
}

std::map<std::string, std::string> ContentParserLinux::getOptions() const {
	return this->options;
}

std::string ContentParserLinux::getOption(std::string const& name) const {
	return this->options.at(name);
}

void ContentParserLinux::setOption(std::string const& name, std::string const& value) {
	this->options[name] = value;
}

std::string ContentParserLinux::buildSource() const {

}
