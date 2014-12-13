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

#ifndef CONTENT_PARSER_LINUXISO_H_
#define CONTENT_PARSER_LINUXISO_H_

#include "../Regex.hpp"
#include "../../Model/DeviceMap.hpp"
#include "Abstract.hpp"

class ContentParser_LinuxIso :
	public ContentParser_Abstract,
	public Regex_RegexConnection,
	public Model_DeviceMap_Connection
{
	static const char* _regex;
	std::string sourceCode;
public:
	void parse(std::string const& sourceCode) {
		this->sourceCode = sourceCode;
		try {
			std::vector<std::string> result = this->regexEngine->match(ContentParser_LinuxIso::_regex, this->sourceCode);
	
			//check partition indices by uuid
			Model_DeviceMap_PartitionIndex pIndex = this->deviceMap->getHarddriveIndexByPartitionUuid(result[3]);
			if (pIndex.hddNum != result[1] || pIndex.partNum != result[2]){
				throw ParserException("parsing failed - hdd num check", __FILE__, __LINE__);
			}
	
			//check if the iso filepaths are the same
			if (result[4] != result[6])
				throw ParserException("parsing failed - iso filepaths are different", __FILE__, __LINE__);
	
			//assign data
			this->options["partition_uuid"] = result[3];
			this->options["linux_image"] = result[5];
			this->options["initramfs"] = result[9];
			this->options["iso_path"] = result[4];
			this->options["locale"] = result[7];
			this->options["other_params"] = result[8];
		} catch (RegExNotMatchedException const& e) {
			throw ParserException("parsing failed - RegEx not matched", __FILE__, __LINE__);
		}
	}

	std::string buildSource() const {
		Model_DeviceMap_PartitionIndex pIndex = this->deviceMap->getHarddriveIndexByPartitionUuid(this->options.at("partition_uuid"));
		std::map<int, std::string> newValues;
		newValues[1] = pIndex.hddNum;
		newValues[2] = pIndex.partNum;
		newValues[3] = this->options.at("partition_uuid");
		newValues[4] = this->options.at("iso_path");
		newValues[5] = this->options.at("linux_image");
		newValues[6] = this->options.at("iso_path");
		newValues[7] = this->options.at("locale");
		newValues[8] = this->options.at("other_params");
		newValues[9] = this->options.at("initramfs");
	
		std::string result = this->regexEngine->replace(ContentParser_LinuxIso::_regex, this->sourceCode, newValues);
	
		//check the new string. If they aren't matchable anymore (evil input), do a rollback
		try {
			this->regexEngine->match(ContentParser_LinuxIso::_regex, result);
		} catch (RegExNotMatchedException const& e) {
			this->log("Ignoring data - doesn't match", Logger::ERROR);
			result = this->sourceCode;
		}
		return result;
	}


	void buildDefaultEntry(std::string const& partition_uuid) {
		std::string defaultEntry = "\
		set root='(hd0,0)'\n\
		search --no-floppy --fs-uuid --set=root 000000000000000000\n\
		loopback loop /xxx.iso\n\
		linux (loop)/casper/vmlinuz boot=casper iso-scan/filename=/xxx.iso quiet splash locale=en_US bootkbd=us console-setup/layoutcode=us noeject --\n\
		initrd (loop)/casper/initrd.lz\n";
		Model_DeviceMap_PartitionIndex pIndex = this->deviceMap->getHarddriveIndexByPartitionUuid(partition_uuid);
		std::map<int, std::string> newValues;
		newValues[1] = pIndex.hddNum;
		newValues[2] = pIndex.partNum;
		newValues[3] = partition_uuid;
	//	newValues[4] = ISO
	//	newValues[5] = KERNEL
	//	newValues[6] = ISO
	//	newValues[7] = LOCALE
	//	newValues[8] = INITRD
	
		this->parse(this->regexEngine->replace(ContentParser_LinuxIso::_regex, defaultEntry, newValues));
	}

};

const char* ContentParser_LinuxIso::_regex = "\
[ \t]*set root='\\(hd([0-9]+)[^0-9]+([0-9]+)\\)'\\n\
[ \t]*search[ \\t]+--no-floppy[ \\t]+--fs-uuid[ \\t]+--set(?:=root)? ([-0-9a-fA-F]+)\\n\
[ \t]*loopback[ \\t]+loop[ \t]+([^ \\t]+)\\n\
[ \t]*linux[ \\t]+\\(loop\\)([^ \\t]+)[ \\t]+boot=casper iso-scan/filename=([^ \\t]+) quiet splash locale=([^ \\t]+) (.*)\\n\
[ \t]*initrd[ \\t]+\\(loop\\)([^ \\t]+)\\n\
";

#endif /* CONTENT_PARSER_LINUXISO_H_ */
