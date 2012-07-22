#ifndef SELECT_ROOT_DLG_GTK_INCLUDED
#define SELECT_ROOT_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include "../model/deviceDataList.h"
#include "../model/mountTable.h"
#include "../interface/evt_partitionChooser.h"
#include <libintl.h>
#include <locale.h>
#include <iostream>
#include "../config.h"
#include "../interface/partitionChooser.h"

/**
 * Gtk::Assistant seem to be buggy: after applying the assistant, a call of show() brings a broken (empty)
 * Window until the user clicks next. The only solution seems to be in creating a new dialog.
 * So I didn't derive Gtk::Assistant but using a pointer to such an object. This allows to
 * create a new instance while the presenter simply needs to call show() or hide() on the PartitionChooser.
 */

//TODO: don't use run() to start this (only if standalone)
class PartitionChooserGtk : public PartitionChooser {
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
	PartitionChooserGtk(bool isLiveCD);
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
	void addPartitionSelectorItem(Glib::ustring const& device, Glib::ustring const& type, Glib::ustring const& label);
	void clearPartitionSelector();
	void addSubmountpoint(std::string const& mountpoint, bool isMounted);
	void removeAllSubmountpoints();
	void submountpoint_toggle(Gtk::CheckButton& sender);
	void showErrorMessage(MountExceptionType type);
	void run();
	void hide();
	void show();
	Gtk::CheckButton& getSubmountpointCheckboxByLabel(Glib::ustring const& label);
	void setSubmountpointSelectionState(Glib::ustring const& submountpoint, bool new_isSelected);
	void setIsMounted(bool isMounted);
};
#endif

