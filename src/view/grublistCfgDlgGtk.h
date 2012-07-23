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

#ifndef GRUBLIST_CFG_DLG_GTK_INCLUDED
#define GRUBLIST_CFG_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include "../interface/evt_listCfgDlg.h"
#include <libintl.h>
#include "../config.h"
#include "../interface/grublistCfgDlg.h"
#include "../presenter/commonClass.h"
#include "settingsDlgGtk.h"
#include <cassert>

//TODO: Edit -> Rename [v3]
//TODO: Edit -> activate/unactivate [v3]
//TODO: Script descriptions (to avoid two custom entries) [v3]
class GrubConfListing : public Gtk::TreeView {
public:
	struct TreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> text;
		Gtk::TreeModelColumn<void*> relatedRule;
		Gtk::TreeModelColumn<bool> is_other_entries_marker;
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
	GrubConfListing();
};

class ImageMenuItemOwnKey : public Gtk::ImageMenuItem {
	public:
	ImageMenuItemOwnKey(const Gtk::StockID& id, const Gtk::AccelKey& accel_key);
};

class GrublistCfgDlgGtk : public GrublistCfgDlg, public CommonClass {
	EventListener_listCfgDlg* eventListener;
	Gtk::Window win;
	Gtk::VBox vbMainSplit;
	Gtk::Notebook notebook;
	
	Gtk::MenuBar menu;
	Gtk::Toolbar toolbar;
	Gtk::ScrolledWindow scrEntryList;
	Gtk::Statusbar statusbar;
	
	GrubConfListing tvConfList;
	Gtk::ProgressBar progressBar;

	Gtk::ToolButton tbttAdd, tbttRemove, tbttUp, tbttDown, tbttSave, tbttReload, tbttLeft, tbttRight;
	Gtk::ToolItem ti_sep1;
	Gtk::VSeparator vs_sep1;
	Gtk::ToolItem ti_sep2;
	Gtk::VSeparator vs_sep2;
	Gtk::ToolItem ti_sep3;
	Gtk::VSeparator vs_sep3;
	Gtk::ToolItem ti_sep4;
	Gtk::VSeparator vs_sep4;
	
	Gtk::MenuItem miFile, miEdit, miView, miHelp, miInstallGrub;
	Gtk::ImageMenuItem miExit, miSave, miAbout, miStartRootSelector;
	ImageMenuItemOwnKey miReload, miAdd, miRemove, miUp, miDown, miLeft, miRight;
	Gtk::Menu subFile, subEdit, subView, subHelp;
	
	Gtk::VBox settingsHBox;

	bool lock_state;

	Gtk::MessageDialog burgSwitcher, pchooserQuestionDlg;

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
	GrublistCfgDlgGtk();
	void setEventListener(EventListener_listCfgDlg& eventListener);
	void putSettingsDialog(Gtk::VBox& commonSettingsPane, Gtk::VBox& appearanceSettingsPane);
	void show();
	void run();
	void close();
	void showBurgSwitcher();
	void hideBurgSwitcher();
	void showPartitionChooserQuestion();
	void hidePartitionChooserQuestion();
	bool isVisible();
	
	void setIsBurgMode(bool isBurgMode);
	
	void setLockState(int state);
	
	void setProgress(double progress);
	void progress_pulse();
	void hideProgressBar();
	void setStatusText(std::string const& new_status_text);
	void appendEntry(std::string const& name, void* entryPtr, bool is_placeholder, bool is_submenu, std::string const& scriptName, std::string const& defaultName, void* parentEntry = NULL);
	void showProxyNotFoundMessage();
	std::string createNewEntriesPlaceholderString(std::string const& parentMenu = "", std::string const& sourceScriptName = "");
	std::string createPlaintextString() const;
	
	void setDefaultTitleStatusText(std::string const& str);
	
	int showExitConfirmDialog(int type);
	void showErrorMessage(std::string const& msg, std::vector<std::string> const& values);
	void clear();
	
	std::string getRuleName(void* rule);
	void setRuleName(void* rule, std::string const& newName);

	void selectRule(void* rule, bool startEdit = false);

	void selectRules(std::list<void*> rules);

private:
	//event handlers
	void signal_show_root_selector();
	void signal_edit_name_finished(const Glib::ustring& path, const Glib::ustring& new_text);
	void signal_move_click(int direction); //direction: -1: one position up, 1: one p. down
	void signal_add_click();
	void signal_remove_click();
	void signal_reload_click();
	void signal_show_grub_install_dialog_click();
	void signal_move_left_click();
	void signal_move_right_click();
	void signal_treeview_selection_changed();
	bool signal_delete_event(GdkEventAny* event);
	void signal_quit_click();
	void signal_preference_click();
	void signal_info_click();
	void signal_burg_switcher_response(int response_id);
	void signal_partition_chooser_question_response(int response_id);
	void signal_edit_name(Gtk::CellEditable* editable, const Glib::ustring& path);
};

#endif
