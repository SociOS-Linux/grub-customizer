#ifndef SELECT_ROOT_DLG_INCLUDED
#define SELECT_ROOT_DLG_INCLUDED
#include <gtkmm.h>
#include "deviceDataList.h"
#include "mountTable.h"
#include "umount_all.h"
#include <libintl.h>
#include <locale.h>
#include <iostream>
class LiveCDSetupDialog : public Gtk::Assistant {
	Gtk::Label lblMessage, lblSubmountpointDescription;
	Gtk::ListViewText lvRootPartitionSelection;
	Gtk::ScrolledWindow scrRootPartitionSelection;
	Gtk::HBox hbCustomPartition;
	Gtk::CheckButton chkCustomPartition;
	Gtk::Entry txtCustomPartition;
	Gtk::VBox vbIntroPage, vbRootSelectPage, vbAdditionalMountSelectionPage;
	Gtk::ScrolledWindow scrAdditionalMountSelectionPage;
	Gtk::VBox vbAdditionalMountSelectionPageList;
	std::string mountpoint;
	Gtk::HBox hbMountButtons;
	Gtk::Button bttMountFs, bttUmountFs;
	bool submountpoint_toggle_run_event;
	bool is_cancelled;
	public:
	LiveCDSetupDialog(bool isLiveCD);
	void updateSensitivity();
	void readPartitionInfo();
	void generateSubmountpointSelection(FILE* fstabFile);
	void signal_custom_partition_toggled();
	void signal_lvRootPartitionSelection_changed();
	void signal_custom_partition_typing();
	void signal_btt_mount_click();
	void signal_btt_umount_click();
	void on_cancel();
	void on_apply();
	void submountpoint_toggle(Gtk::CheckButton& sender);
	std::string getRootMountpoint() const;
	bool isCancelled() const;
};
#endif

