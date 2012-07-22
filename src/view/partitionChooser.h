#ifndef SELECT_ROOT_DLG_INCLUDED
#define SELECT_ROOT_DLG_INCLUDED
#include <gtkmm.h>
#include "../model/deviceDataList.h"
#include "../model/mountTable.h"
#include "../interface/eventListener_view_iface.h"
#include <libintl.h>
#include <locale.h>
#include <iostream>
#include "../config.h"
class PartitionChooser : public Gtk::Assistant {
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
	EventListenerView_iface* eventListener;
	MountTable rootFstab;
	public:
	PartitionChooser(bool isLiveCD);
	void setEventListener(EventListenerView_iface& eventListener);
	void updateSensitivity();
	void readPartitionInfo();
	void generateSubmountpointSelection(std::string const& prefix);
	void signal_custom_partition_toggled();
	void signal_lvRootPartitionSelection_changed();
	void signal_custom_partition_typing();
	void signal_btt_mount_click();
	void signal_btt_umount_click();
	void on_cancel();
	void on_apply();

	std::string getSelectedDevice();
	void addSubmountpoint(std::string const& mountpoint, bool isMounted);
	void removeAllSubmountpoints();
	void submountpoint_toggle(Gtk::CheckButton& sender);
	std::string getRootMountpoint() const;
	bool isCancelled() const;
	Glib::ustring run();
};
#endif

