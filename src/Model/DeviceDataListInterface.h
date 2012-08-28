#ifndef DEVICEDATALIST_IFACE_H_
#define DEVICEDATALIST_IFACE_H_
#include <map>
#include <string>

class Model_DeviceDataListInterface : public std::map<std::string, std::map<std::string, std::string> > {
public:
	virtual inline ~Model_DeviceDataListInterface() {};

	virtual void loadData(FILE* blkidOutput)=0;
	virtual void clear()=0;
};

#endif
