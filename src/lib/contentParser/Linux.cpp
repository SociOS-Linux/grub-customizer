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

ContentParserLinux::ContentParserLinux(GrubDeviceMap& deviceMap)
	: deviceMap(deviceMap)
{}

void ContentParserLinux::parse(std::string const& sourceCode) {
	this->sourceCode = sourceCode;
	try {
		std::vector<std::string> result = Regex::match(ContentParserLinux::_regex, sourceCode);

		//check partition indices by uuid
		GrubPartitionIndex pIndex = deviceMap.getHarddriveIndexByPartitionUuid(result[6]);
		if (pIndex.hddNum != result[1] || pIndex.partNum != result[2]){
			throw ContentParser::PARSING_FAILED;
		}

		//check if the uuids (Kernel <-> search command) are the same
		if (result[3] != result[6])
			throw ContentParser::PARSING_FAILED;

		//assign data
		this->options["partition_uuid"] = result[6];
		this->options["linux_image"] = result[5];
		this->options["initramfs"] = result[8];
	} catch (Regex::Exception e) {
		throw ContentParser::PARSING_FAILED;
	}
}

std::string ContentParserLinux::buildSource() const {
	GrubPartitionIndex pIndex = deviceMap.getHarddriveIndexByPartitionUuid(this->options.at("partition_uuid"));
	std::map<int, std::string> newValues;
	newValues[1] = pIndex.hddNum;
	newValues[2] = pIndex.partNum;
	newValues[3] = this->options.at("partition_uuid");
	newValues[5] = this->options.at("linux_image");
	newValues[6] = this->options.at("partition_uuid");
	newValues[8] = this->options.at("initramfs");

	std::string newSourceCode = Regex::replace(ContentParserLinux::_regex, this->sourceCode, newValues);

	//check the new string. If they aren't matchable anymore (evil input), do a rollback
	try {
		Regex::match(ContentParserLinux::_regex, newSourceCode);
		return newSourceCode;
	} catch (Regex::Exception e) {
		this->log("Ignoring data - doesn't match", Logger::ERROR);
		return this->sourceCode;
	}
}

void ContentParserLinux::buildDefaultEntry(std::string const& partition_uuid) {
	std::string defaultEntry = "\
	set root='(hd0,0)'\n\
	search --no-floppy --fs-uuid --set 000000000000\n\
	linux /vmlinuz root=UUID=000000000000 \n\
	initrd /initrd.img";
	GrubPartitionIndex pIndex = this->deviceMap.getHarddriveIndexByPartitionUuid(partition_uuid);
	std::map<int, std::string> newValues;
	newValues[1] = pIndex.hddNum;
	newValues[2] = pIndex.partNum;
	newValues[3] = partition_uuid;
	newValues[6] = partition_uuid;

	this->parse(Regex::replace(ContentParserLinux::_regex, defaultEntry, newValues));
}
