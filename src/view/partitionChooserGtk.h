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

#ifndef SELECT_ROOT_DLG_GTK_INCLUDED
#define SELECT_ROOT_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include "../model/deviceDataList.h"
#include "../model/mountTable.h"
#include "../interface/evt_partitionChooser.h"
#include <libintl.h>
#include <locale.h>
#include "../config.h"
#include "../interface/partitionChooser.h"
#include "../presenter/commonClass.h"

/**
 * Gtk::Assistant seem to be buggy: after applying the assistant, a call of show() brings a broken (empty)
 * Window until the user clicks next. The only solution seems to be in creating a new dialog.
 * So I didn't derive Gtk::Assistant but using a pointer to such an object. This allows to
 * create a new instance while the presenter simply needs to call show() or hide() on the PartitionChooser.
 */

class PartitionChooserGtk : public PartitionChooser, public CommonClass {
	Gtk::Assistant* assistant;
	Gtk::Label lblMessage, lblSubmountpointDescription;
	Gtk::ListViewText lvRootPartitionSelection;
	Gtk::ScrolledWindow scrRootPartitionSelection;
	Gtk::HBox hbCustomPartition;
	Gtk::CheckButton chkCustomPartition;
	Gtk::Entry txtCustomPartition;
	Gtk::VBox vbIntroPage, vbRootSelectPage, vbAdditionalMountSelectionPage;
	Gtk::ScrolledWindow scrAdditionalMountSelectionPage;
	Gtk::VBox vbAdditionalMountSelectionPageList;
	bool isMounted;
	Gtk::HBox hbMountButtons;
	Gtk::Button bttMountFs, bttUmountFs;
	bool submountpoint_toggle_run_event;
	EventListener_partitionChooser* eventListener;
	void init(bool useExisting = true);
	Gtk::Assistant& getWindow();
	public:
	PartitionChooserGtk();
	void setEventListener(EventListener_partitionChooser& eventListener);
	void updateSensitivity();
	void signal_custom_partition_toggled();
	void signal_lvRootPartitionSelection_changed();
	void signal_custom_partition_typing();
	void signal_btt_mount_click();
	void signal_btt_umount_click();
	void on_cancel();
	void on_apply();

	std::string getSelectedDevice();
	void addPartitionSelectorItem(std::string const& device, std::string const& type, std::string const& label);
	void clearPartitionSelector();
	void addSubmountpoint(std::string const& mountpoint, bool isMounted);
	void removeAllSubmountpoints();
	void submountpoint_toggle(Gtk::CheckButton& sender);
	void showErrorMessage(MountExceptionType type);
	void hide();
	void show();
	Gtk::CheckButton& getSubmountpointCheckboxByLabel(Glib::ustring const& label);
	void setSubmountpointSelectionState(std::string const& submountpoint, bool new_isSelected);
	void setIsMounted(bool isMounted);
	void setIsStartedManually(bool val);
};
#endif

