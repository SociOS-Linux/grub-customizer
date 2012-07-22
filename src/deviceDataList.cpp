#include "deviceDataList.h"

DeviceDataList::DeviceDataList(FILE* blkidOutput){
	loadData(blkidOutput);
}

DeviceDataList::DeviceDataList(){}

void DeviceDataList::loadData(FILE* blkidOutput){
	std::string deviceName, attributeName;
	bool inAttributeValue = false;
	bool deviceNameIsComplete = false, attributeNameIsComplete = false;
	int c;
	while ((c = fgetc(blkidOutput)) != EOF){
		if (inAttributeValue && c != '"'){
			(*this)[deviceName][attributeName] += c;
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
}
