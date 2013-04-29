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

#include "DeviceMap.h"

Model_SmartFileHandle Model_DeviceMap::getFileHandle() const {
	Model_SmartFileHandle result;
	try {
		result.open(env->devicemap_file, "r", Model_SmartFileHandle::TYPE_FILE);
	} catch (FileReadException const& e) {
		result.open(env->mkdevicemap_cmd, "r", Model_SmartFileHandle::TYPE_COMMAND);
	}
	return result;
}

Model_DeviceMap_PartitionIndex Model_DeviceMap::getHarddriveIndexByPartitionUuid(std::string partitionUuid) const {
	if (this->_cache.find(partitionUuid) != this->_cache.end()) {
		return this->_cache[partitionUuid];
	}

	Model_DeviceMap_PartitionIndex result;
	char deviceBuf[101];
	int size = readlink((this->env->cfg_dir_prefix + "/dev/disk/by-uuid/" + partitionUuid).c_str(), deviceBuf, 100);
	if (size == -1) { //if this didn't work, try to convert the uuid to uppercase
		for (std::string::iterator iter = partitionUuid.begin(); iter != partitionUuid.end(); iter++)
			*iter = std::toupper(*iter);
		size = readlink((this->env->cfg_dir_prefix + "/dev/disk/by-uuid/" + partitionUuid).c_str(), deviceBuf, 100);
	}
	if (size == -1) {
		return result; //abort with empty result
	}

	deviceBuf[size] = '\0';

	std::vector<std::string> regexResult = Regex::match("([^/.0-9]+)([0-9]+)$", deviceBuf);
	result.partNum = regexResult[2];

	std::string diskDevice = regexResult[1];

	Model_SmartFileHandle handle = this->getFileHandle();

	try {
		while (result.hddNum == "") {
			std::string row = handle.getRow();
			std::vector<std::string> rowMatch = Regex::match("^\\(hd([0-9]+)\\)\t(.*)$", row);
			if (row.find(diskDevice) != -1) {
				result.hddNum = rowMatch[1];
				break;
			} else { // the files contains a path to a symlink
				int size = readlink(rowMatch[2].c_str(), deviceBuf, 100); // if this is a link, follow it
				if (size != -1) {
					result.hddNum = rowMatch[1];
				}
			}
		}
	} catch (EndOfFileException const& e) {
		//don't throw if we didn't find the mapped value
	}
	handle.close();

	this->_cache[partitionUuid] = result;
	return result;
}
