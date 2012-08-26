/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GRUBENVEDITORGTK_H_
#define GRUBENVEDITORGTK_H_
#include <gtkmm.h>
#include <libintl.h>
#include "../../presenter/commonClass.h"
#include "../EnvEditor.h"
#include "../../interface/evt_grubEnvEditor.h"
#include "Element/PartitionChooser.h"
#include "../../interface/deviceDataList_Iface.h"
#include "../../Controller/EnvEditorController.h"

class View_Gtk_EnvEditor : public Gtk::Dialog, public View_EnvEditor, public CommonClass {
	EnvEditorController* eventListener;
	Gtk::VBox vbContent;
	Gtk::Table tblLayout;
	Gtk::Label lblPartition;
	Gtk::Label lblType;
	Gtk::ComboBoxText cbType;
	Gtk::HSeparator separator;
	Gtk::ScrolledWindow scrSubmountpoints;
	Gtk::VBox vbSubmountpoints;
	Gtk::Label lblSubmountpoints;
	View_Gtk_Element_PartitionChooser* pChooser;
	DeviceDataList_Iface* deviceDataList;
	std::map<std::string, Gtk::Entry*> optionMap;
	std::map<std::string, Gtk::Label*> labelMap;
	std::map<std::string, Gtk::Image*> imageMap;
	std::map<std::string, Gtk::CheckButton*> subMountpoints;
	Gtk::CheckButton cbSaveConfig;
	Gtk::HButtonBox bbxSaveConfig;
	bool eventLock;

	std::string rootDeviceName;
public:
	View_Gtk_EnvEditor();
	~View_Gtk_EnvEditor();
	void setEventListener(EnvEditorController& eventListener);
	void setDeviceDataList(DeviceDataList_Iface& deviceDataList);
	void setRootDeviceName(std::string const& rootDeviceName);
	void setEnvSettings(std::map<std::string, std::string> const& props, std::list<std::string> const& requiredProps, std::list<std::string> const& validProps);
	std::map<std::string, std::string> getEnvSettings();
	int getBootloaderType() const;
	void show(bool resetPartitionChooser = false);
	void hide();
	void removeAllSubmountpoints();
	void addSubmountpoint(std::string const& name, bool isActive);
	void setSubmountpointSelectionState(std::string const& submountpoint, bool new_isSelected);
	void showErrorMessage(MountExceptionType type);
	Gtk::Widget& getContentBox();
private:
	void signal_partitionChanged();
	void signal_bootloaderType_changed();
	void signal_optionModified();
	void signal_response_action(int response_id);
	void signal_submountpointToggled(Gtk::CheckButton& sender);
};

#endif /* GRUBENVEDITOR_H_ */
