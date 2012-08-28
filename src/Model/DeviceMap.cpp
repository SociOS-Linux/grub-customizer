#include "DeviceMap.h"

Model_DeviceMap::Model_DeviceMap(Model_Env const& env) {
	this->env = &env;
}
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
