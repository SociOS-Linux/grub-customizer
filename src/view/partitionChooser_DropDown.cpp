#include "partitionChooser_DropDown.h"

PartitionChooser_DropDown::PartitionChooser_DropDown(Glib::ustring const& activePartition_uuid, DeviceDataList_Iface const& deviceDataList)
	: activePartition_uuid(activePartition_uuid), deviceDataList(&deviceDataList)
{
	load();
}

std::string PartitionChooser_DropDown::strToLower(std::string str) {
	for (std::string::iterator iter = str.begin(); iter != str.end(); iter++) {
		*iter = std::tolower(*iter);
	}
	return str;
}

/**
 * returns the uuid of the selected entry of the combobox
 */
std::string PartitionChooser_DropDown::getSelectedUuid() const {
	return this->uuid_map.at(this->get_active_text());
}

void PartitionChooser_DropDown::load(){
	this->clear();
	for (DeviceDataList_Iface::const_iterator iter = deviceDataList->begin(); iter != deviceDataList->end(); iter++) {
		Glib::ustring text = iter->first + "\n(" + (iter->second.find("LABEL") != iter->second.end() ? iter->second.at("LABEL") + ", " : "") + (iter->second.find("TYPE") != iter->second.end() ? iter->second.at("TYPE") : "") + ")";
		uuid_map[text] = iter->second.at("UUID");
		this->append_text(text);
		if (strToLower(iter->second.at("UUID")) == strToLower(activePartition_uuid) || activePartition_uuid == "" && iter == deviceDataList->begin()) {
			this->set_active_text(text);
		}
	}
}
