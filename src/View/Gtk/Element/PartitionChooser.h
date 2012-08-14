#ifndef PARTITIONCHOOSER_DROPDOWN_H_
#define PARTITIONCHOOSER_DROPDOWN_H_
#include <gtkmm.h>
#include <string>
#include "../../../interface/deviceDataList_Iface.h"
#include <libintl.h>

class View_Gtk_Element_PartitionChooser : public Gtk::ComboBoxText {
	std::map<std::string, std::string> uuid_map;
	Glib::ustring activePartition_uuid;
	DeviceDataList_Iface const* deviceDataList;
	bool prependCurrentPartition;
	std::string currentPartitionName;
public:
	View_Gtk_Element_PartitionChooser(Glib::ustring const& activePartition_uuid, DeviceDataList_Iface const& deviceDataList, bool prependCurrentPartition = false, std::string const& currentPartitionName = "");
	void load();
	std::string getSelectedUuid() const;
	static std::string strToLower(std::string str);
};

#endif
