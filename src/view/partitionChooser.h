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
//TODO: fix blank window when Gtk::Assistant will be reopened
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
	bool isMounted;
	Gtk::HBox hbMountButtons;
	Gtk::Button bttMountFs, bttUmountFs;
	bool submountpoint_toggle_run_event;
	EventListenerView_iface* eventListener;
	public:
	bool is_cancelled;
	PartitionChooser(bool isLiveCD);
	void setEventListener(EventListenerView_iface& eventListener);
	void updateSensitivity();
	void signal_custom_partition_toggled();
	void signal_lvRootPartitionSelection_changed();
	void signal_custom_partition_typing();
	void signal_btt_mount_click();
	void signal_btt_umount_click();
	void on_cancel();
	void on_apply();

	std::string getSelectedDevice();
	void addPartitionSelectorItem(Glib::ustring const& device, Glib::ustring const& type, Glib::ustring const& label);
	void clearPartitionSelector();
	void addSubmountpoint(std::string const& mountpoint, bool isMounted);
	void removeAllSubmountpoints();
	void submountpoint_toggle(Gtk::CheckButton& sender);
	void showErrorMessage(MountException::Type type);
	void run();
	Gtk::CheckButton& getSubmountpointCheckboxByLabel(Glib::ustring const& label);
	void setSubmountpointSelectionState(Glib::ustring const& submountpoint, bool new_isSelected);
	void setIsMounted(bool isMounted);
};
#endif

