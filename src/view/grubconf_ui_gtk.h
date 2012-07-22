#ifndef GRUBCONF_UI_GTK_INCLUDED
#define GRUBCONF_UI_GTK_INCLUDED
#include <gtkmm.h>
#include "../interface/grubconf_ui.h"
#include "../model/grublistCfg.h"
#include "partitionChooser.h"
#include "settings_dlg_gtk.h"
#include <iostream>
#include "../interface/eventListener_view_iface.h"

#include <libintl.h>


class GrubConfListing : public Gtk::TreeView {
public:
	struct TreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<bool> active;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Rule*> relatedRule;
		Gtk::TreeModelColumn<Proxy*> relatedProxy;
		Gtk::TreeModelColumn<bool> is_other_entries_marker;
		Gtk::TreeModelColumn<bool> is_editable;
		TreeModel();
	};
	TreeModel treeModel;
	Glib::RefPtr<Gtk::TreeStore> refTreeStore;
	GrubConfListing();
};

class ImageMenuItemOwnKey : public Gtk::ImageMenuItem {
	public:
	ImageMenuItemOwnKey(const Gtk::StockID& id, const Gtk::AccelKey& accel_key);
};

class GrubConfUIGtk : public GrubConfUI {
	GrublistCfg* grubConfig;
	EventListenerView_iface* eventListener;
	Gtk::Window win;
	Gtk::VBox vbMainSplit;
	
	Gtk::MenuBar menu;
	Gtk::Toolbar toolbar;
	Gtk::ScrolledWindow scrEntryList;
	Gtk::Statusbar statusbar;
	
	GrubConfListing tvConfList;
	Gtk::ProgressBar progressBar;
	Glib::Dispatcher disp_update_load, disp_update_save, disp_thread_died, disp_grub_install_ready;

	Glib::ustring appName, appVersion;
	std::vector<Glib::ustring> authors;

	Gtk::ToolButton tbttAdd, tbttRemove, tbttUp, tbttDown, tbttSave, tbttPreferences, tbttReload;
	Gtk::ToolItem ti_sep1;
	Gtk::VSeparator vs_sep1;
	Gtk::ToolItem ti_sep2;
	Gtk::VSeparator vs_sep2;
	Gtk::ToolItem ti_sep3;
	Gtk::VSeparator vs_sep3;
	Gtk::ToolItem ti_sep4;
	Gtk::VSeparator vs_sep4;
	
	Gtk::MenuItem miFile, miEdit, miView, miHelp, miInstallGrub;
	Gtk::ImageMenuItem miExit, miSave, miPreferences, miAbout, miStartRootSelector;
	ImageMenuItemOwnKey miReload, miAdd, miRemove, miUp, miDown;
	Gtk::Menu subFile, subEdit, subView, subHelp;
	Gtk::AboutDialog dlgAbout;
	
	bool completelyLoaded;
	bool quit_requested;
	public: bool modificationsUnsaved, thread_active; //TODO: move to presenter!
	private:
	//Script/Proxy-add-Window
	Gtk::Dialog scriptAddDlg;
	Gtk::VBox vbScriptPreview;
	Gtk::HBox hbScriptSelection;
	Gtk::ComboBoxText cbScriptSelection;
	Gtk::ListViewText lvScriptPreview;
	Gtk::ScrolledWindow scrScriptPreview;
	Gtk::Label lblScriptSelection;
	Gtk::Label lblScriptPreview;
	
	//Grub Install Dialog
	Gtk::Dialog grubInstallDialog;
	Gtk::Label lblGrubInstallDescription;
	Gtk::HBox hbGrubInstallDevice;
	Gtk::Label lblGrubInstallDevice, lblInstallInfo;
	Gtk::Entry txtGrubInstallDevice;
public:
	GrubConfUIGtk(GrublistCfg& grubConfig);
	void setEventListener(EventListenerView_iface& eventListener);
	void run();
	void update();
	void update_save();
	void thread_died_handler();
	void event_load_progress_changed();
	void event_save_progress_changed();
	void event_thread_died();
	void event_grub_install_ready();
	void event_mode_changed();
	bool bootloader_not_found_requestForRootSelection();
	std::string show_root_selector();
	bool requestForBurgMode();
	
	void reload(bool keepConfig);
	void func_disp_grub_install_ready();
	void signal_show_root_selector();
	void signal_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
	void signal_move_click(int direction); //direction: -1: one position up, 1: one p. down
	void signal_add_click();
	void signal_remove_click();
	void signal_reload_click();
	void signal_script_selection_changed();
	void signal_scriptAddDlg_response(int response_id);
	void signal_show_grub_install_dialog_click();
	void signal_treeview_selection_changed();
	bool signal_delete_event(GdkEventAny* event);
	void signal_about_dlg_response(int response_id);
	void signal_quit_click();
	void signal_grub_install_dialog_response(int response_id);
	void signal_preference_click();
public:
	void update_move_buttons();
	void update_remove_button();
	void configureOtherEntriesMarker(Gtk::TreeIter otherEntriesMarker);
	void saveConfig();
	void setLockState(int state);
	void updateButtonsState();
};

#endif
