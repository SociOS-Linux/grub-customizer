#ifndef GRUBDEVICEMAP_H_
#define GRUBDEVICEMAP_H_
#include "SmartFileHandle.h"
#include "Env.h"
#include "../lib/regex.h"
#include <map>

struct Model_DeviceMap_PartitionIndex {
	std::string hddNum, partNum;
	operator std::string();
};

class Model_DeviceMap {
	Model_Env const * env;
	mutable std::map<std::string, Model_DeviceMap_PartitionIndex> _cache;
public:
	Model_DeviceMap(Model_Env const& env);
	Model_SmartFileHandle getFileHandle() const;
	Model_DeviceMap_PartitionIndex getHarddriveIndexByPartitionUuid(std::string partitionUuid) const;
};

#endif
