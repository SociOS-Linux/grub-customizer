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
#include "Memtest.h"

const char* ContentParserMemtest::_regex = "\
[ \t]*set root='\\(hd([0-9]+)[^0-9]+([0-9]+)\\)'\\n\
[ \t]*search[ \t]+--no-floppy[ \t]+--fs-uuid[ \t]+--set(?:=root)? ([-0-9a-fA-F]+)\\n\
[ \t]*linux16[ \t]*([^ \\t\\n]+).*$\
";

ContentParserMemtest::ContentParserMemtest(GrubDeviceMap& deviceMap)
	: deviceMap(deviceMap)
{}

void ContentParserMemtest::parse(std::string const& sourceCode) {
	this->sourceCode = sourceCode;
	try {
		std::vector<std::string> result = Regex::match(ContentParserMemtest::_regex, this->sourceCode);


		//check partition indices by uuid
		GrubPartitionIndex pIndex = deviceMap.getHarddriveIndexByPartitionUuid(result[3]);
		if (pIndex.hddNum != result[1] || pIndex.partNum != result[2]){
			throw ContentParser::PARSING_FAILED;
		}

		this->options["partition_uuid"] = result[3];
		this->options["memtest_image"] = result[4];
	} catch (Regex::Exception e) {
		throw ContentParser::PARSING_FAILED;
	}
}

std::string ContentParserMemtest::buildSource() const {
	GrubPartitionIndex pIndex = deviceMap.getHarddriveIndexByPartitionUuid(this->options.at("partition_uuid"));
	std::map<int, std::string> newValues;
	newValues[1] = pIndex.hddNum;
	newValues[2] = pIndex.partNum;
	newValues[3] = this->options.at("partition_uuid");
	newValues[4] = this->options.at("memtest_image");

	std::string newSourceCode = Regex::replace(ContentParserMemtest::_regex, this->sourceCode, newValues);

	//check the new string. If they aren't matchable anymore (evil input), do a rollback
	try {
		Regex::match(ContentParserMemtest::_regex, newSourceCode);
		return newSourceCode;
	} catch (Regex::Exception e) {
		this->log("Ignoring data - doesn't match", Logger::ERROR);
		return this->sourceCode;
	}
}

void ContentParserMemtest::buildDefaultEntry(std::string const& partition_uuid) {
	std::string defaultEntry = "\
	set root='(hd0,0)'\n\
	search --no-floppy --fs-uuid --set 000000000000\n\
	linux16 /boot/memtest86+.bin";
	GrubPartitionIndex pIndex = this->deviceMap.getHarddriveIndexByPartitionUuid(partition_uuid);
	std::map<int, std::string> newValues;
	newValues[1] = pIndex.hddNum;
	newValues[2] = pIndex.partNum;
	newValues[3] = partition_uuid;

	this->parse(Regex::replace(ContentParserMemtest::_regex, defaultEntry, newValues));
}
