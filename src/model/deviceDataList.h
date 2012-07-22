#ifndef DEVICE_DATALIST_INCLUDED
#define DEVICE_DATALIST_INCLUDED
#include <map>
#include <cstdio>
#include <string>
class DeviceDataList : public std::map<std::string, std::map<std::string, std::string> > {
public:
	DeviceDataList(FILE* blkidOutput);
	DeviceDataList();
	void loadData(FILE* blkidOutput);
	void clear();
};
#endif
