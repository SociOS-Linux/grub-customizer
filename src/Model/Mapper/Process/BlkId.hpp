/*
 * Copyright (C) 2010-2014 Daniel Richter <danielrichter2007@web.de>
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

#ifndef SRC_MODEL_MAPPER_PROCESS_BLKID_HPP_
#define SRC_MODEL_MAPPER_PROCESS_BLKID_HPP_

#include <map>
#include <cstdio>
#include <string>
#include "../../Data/Mountpoints/DeviceDataList.hpp"

class Model_Mapper_Process_BlkId
{
	/**
	 * loads data from blkid
	 */
	public: void loadData(Model_Data_Mountpoints_DeviceDataList& dest) const
	{
		FILE* blkidProc = popen("blkid", "r");

		if (!blkidProc){
			throw CommandNotFoundException("unable to run blkid");
		}

		dest.clear();

		std::string deviceName, attributeName;
		bool inAttributeValue = false;
		bool deviceNameIsComplete = false, attributeNameIsComplete = false;
		int c;
		while ((c = fgetc(blkidProc)) != EOF){
			if (inAttributeValue && c != '"'){
				dest.getProperty(deviceName, attributeName) += c;
			}
			else {
				if (c == '\n'){
					deviceName = "";
					deviceNameIsComplete = false;
				}
				else if (c == ':'){
					deviceNameIsComplete = true;
				}
				else if (!deviceNameIsComplete) {
					deviceName += c;
				}
				else if (c != '=' && !attributeNameIsComplete) {
					if (c != ' ')
						attributeName += c;
				}
				else if (c == '=')
					attributeNameIsComplete = true;
				else if (c == '"'){
					if (inAttributeValue){
						attributeName = "";
						attributeNameIsComplete = false;
					}
					inAttributeValue = !inAttributeValue;
				}
			}
		}

		pclose(blkidProc);
	}
};

#endif /* SRC_MODEL_MAPPER_PROCESS_BLKID_HPP_ */
