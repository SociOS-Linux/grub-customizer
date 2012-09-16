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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef GRUBLIST_CFG_DLG_GTK_INCLUDED
#define GRUBLIST_CFG_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include <libintl.h>
#include "../../config.h"
#include "../Main.h"
#include "../../lib/CommonClass.h"
#include "Settings.h"
#include "../../lib/str_replace.h"
#include "../../lib/assert.h"

class View_Gtk_Main_List : public Gtk::TreeView {
public:
	struct TreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> text;
		Gtk::TreeModelColumn<void*> relatedRule;
		Gtk::TreeModelColumn<bool> is_other_entries_marker;
		Gtk::TreeModelColumn<bool> is_renamable;
		Gtk::TreeModelColumn<bool> is_editable;
		Gtk::TreeModelColumn<bool> is_sensitive;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
		TreeModel();
	};
	TreeModel treeModel;
	Glib::RefPtr<Gtk::TreeStore> refTreeStore;
	Gtk::CellRendererPixbuf pixbufRenderer;
	Gtk::CellRendererText textRenderer;
	Gtk::TreeViewColumn mainColumn;
	View_Gtk_Main_List();
};

class ImageMenuItemOwnKey : public Gtk::ImageMenuItem {
	public:
	ImageMenuItemOwnKey(const Gtk::StockID& id, const Gtk::AccelKey& accel_key);
};

class View_Gtk_Main : public View_Main, public CommonClass {
	MainController* eventListener;
	Gtk::Window win;
	Gtk::VBox vbMainSplit;
	Gtk::Notebook notebook;
	
	Gtk::MenuBar menu;
	Gtk::Toolbar toolbar;
	Gtk::ScrolledWindow scrEntryList;
	Gtk::InfoBar infoReloadRequired;
	Gtk::Label lblReloadRequired;
	Gtk::VBox vbEntryList;
	Gtk::Statusbar statusbar;
	
	View_Gtk_Main_List tvConfList;
	Gtk::ProgressBar progressBar;

	Gtk::ToolButton tbttAdd, tbttRemove, tbttUp, tbttDown, tbttSave, tbttReload, tbttLeft, tbttRight, tbttEditEntry, tbttRevert, tbttCreateEntry;
	Gtk::ToolItem ti_sep1;
	Gtk::VSeparator vs_sep1;
	Gtk::ToolItem ti_sep2;
	Gtk::VSeparator vs_sep2;
	Gtk::ToolItem ti_sep3;
	Gtk::VSeparator vs_sep3;
	Gtk::ToolItem ti_sep4;
	Gtk::VSeparator vs_sep4;
	Gtk::SeparatorToolItem ti_sep5;
	
	Gtk::MenuItem miFile, miEdit, miView, miHelp, miInstallGrub, miContext;
	Gtk::ImageMenuItem miExit, miSave, miAbout, miModifyEnvironment, miRevert, miCreateEntry;
	ImageMenuItemOwnKey miReload, miAdd, miRemove, miUp, miDown, miLeft, miRight, miEditEntry;
	Gtk::ImageMenuItem miCRemove, miCUp, miCDown, miCLeft, miCRight, miCRename, miCEditEntry;
	Gtk::Menu subFile, subEdit, subView, subHelp, contextMenu;
	
	Gtk::VBox settingsHBox;

	bool lock_state;

	Gtk::MessageDialog burgSwitcher;

	Gtk::Button bttAdvancedSettings1, bttAdvancedSettings2;
	Gtk::HButtonBox bbxAdvancedSettings1, bbxAdvancedSettings2;


	Gtk::TreeModel::iterator getIterByRulePtr(void* rulePtr, const Gtk::TreeRow* parentRow = NULL) const;
	void update_move_buttons();
	void update_remove_button();
	void saveConfig();
	void updateButtonsState();
	bool selectedEntriesAreOnSameLevel();
	bool selectedEntriesAreSubsequent();
	std::list<void*> getSelectedRules();
public:
	View_Gtk_Main();
	void setEventListener(MainController& eventListener);
	void putSettingsDialog(Gtk::VBox& commonSettingsPane, Gtk::VBox& appearanceSettingsPane);
	void putEnvEditor(Gtk::Widget& envEditor);
	void show();
	void hide();
	void run();
	void close();
	void showBurgSwitcher();
	void hideBurgSwitcher();
	bool isVisible();
	
	void setIsBurgMode(bool isBurgMode);
	
	void setLockState(int state);
	
	void setProgress(double progress);
	void progress_pulse();
	void hideProgressBar();
	void setStatusText(std::string const& new_status_text);
	void setStatusText(std::string const& name, int pos, int max);
	void appendEntry(std::string const& name, void* entryPtr, bool is_placeholder, bool is_submenu, std::string const& scriptName, std::string const& defaultName, bool isEditable, bool isModified, std::map<std::string, std::string> const& options, void* parentEntry = NULL);
	void showProxyNotFoundMessage();
	std::string createNewEntriesPlaceholderString(std::string const& parentMenu = "", std::string const& sourceScriptName = "");
	std::string createPlaintextString(std::string const& scriptName) const;
	
	void setDefaultTitleStatusText(std::string const& str);
	
	int showExitConfirmDialog(int type);
	void showErrorMessage(std::string const& msg, std::vector<std::string> const& values);
	bool askForEnvironmentSettings(std::string const& failedCmd, std::string const& errorMessage);
	void clear();
	bool confirmUnsavedSwitch();
	
	std::string getRuleName(void* rule);
	void setRuleName(void* rule, std::string const& newName);

	void selectRule(void* rule, bool startEdit = false);

	void selectRules(std::list<void*> rules);

	void setTrashCounter(int count);

	void showReloadRecommendation();
	void hideReloadRecommendation();

	void showPlaintextRemoveWarning();
	void showSystemRuleRemoveWarning();
private:
	//event handlers
	void signal_show_envEditor();
	void signal_edit_name_finished(const Glib::ustring& path, const Glib::ustring& new_text);
	void signal_move_click(int direction); //direction: -1: one position up, 1: one p. down
	void signal_add_click();
	void signal_remove_click();
	void signal_rename_click();
	void signal_reload_click();
	void signal_show_grub_install_dialog_click();
	void signal_move_left_click();
	void signal_move_right_click();
	void signal_treeview_selection_changed();
	void signal_entry_edit_click();
	void signal_entry_create_click();
	bool signal_delete_event(GdkEventAny* event);
	void signal_quit_click();
	void signal_preference_click();
	void signal_info_click();
	void signal_burg_switcher_response(int response_id);
	void signal_edit_name(Gtk::CellEditable* editable, const Glib::ustring& path);
	void signal_button_press(GdkEventButton *event);
	bool signal_popup();
	void signal_key_press(GdkEventKey* key);
	void signal_revert();
	void signal_reload_recommendation_response(int response_id);
	void signal_tab_changed(GtkNotebookPage* page, guint page_num);
};

#endif
