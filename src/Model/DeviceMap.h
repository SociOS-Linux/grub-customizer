#ifndef GRUBDEVICEMAP_H_
#define GRUBDEVICEMAP_H_
#include "SmartFileHandle.h"
#include "Env.h"
#include "../lib/regex.h"

struct GrubPartitionIndex {
	std::string hddNum, partNum;
	operator std::string();
};

class GrubDeviceMap {
	GrubEnv const * env;
public:
	GrubDeviceMap(GrubEnv const& env);
	SmartFileHandle getFileHandle() const;
	GrubPartitionIndex getHarddriveIndexByPartitionUuid(std::string partitionUuid) const;
};

#endif
