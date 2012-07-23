#ifndef PARTITIONCHOOSER_DROPDOWN_H_
#define PARTITIONCHOOSER_DROPDOWN_H_
#include <gtkmm.h>
#include <string>
#include "../interface/deviceDataList_Iface.h"

class PartitionChooser_DropDown : public Gtk::ComboBoxText {
	std::map<std::string, std::string> uuid_map;
	Glib::ustring activePartition_uuid;
	DeviceDataList_Iface const* deviceDataList;
public:
	PartitionChooser_DropDown(Glib::ustring const& activePartition_uuid, DeviceDataList_Iface const& deviceDataList);
	void load();
	std::string getSelectedUuid() const;
	static std::string strToLower(std::string str);
};

#endif
