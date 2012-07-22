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

//TODO: Edit -> Rename [v3]
//TODO: Edit -> activate/unactivate [v3]
//TODO: Script descriptions (to avoid two custom entries) [v3]
class GrubConfListing : public Gtk::TreeView {
public:
	struct TreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<bool> active;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<void*> relatedRule;
		Gtk::TreeModelColumn<void*> relatedProxy;
		Gtk::TreeModelColumn<bool> is_other_entries_marker;
		Gtk::TreeModelColumn<bool> is_editable;
		Gtk::TreeModelColumn<bool> is_sensitive;
		Gtk::TreeModelColumn<int> font_weight;
		Gtk::TreeModelColumn<Pango::Style> fontStyle;
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

class GrublistCfgDlgGtk : public GrublistCfgDlg, public CommonClass {
	EventListener_listCfgDlg* eventListener;
	Gtk::Window win;
	Gtk::VBox vbMainSplit;
	
	Gtk::MenuBar menu;
	Gtk::Toolbar toolbar;
	Gtk::ScrolledWindow scrEntryList;
	Gtk::Statusbar statusbar;
	
	GrubConfListing tvConfList;
	Gtk::ProgressBar progressBar;

	Gtk::ToolButton tbttAdd, tbttRemove, tbttUp, tbttDown, tbttSave, tbttPreferences, tbttReload, tbttLeft, tbttRight;
	Gtk::ToolItem ti_sep1;
	Gtk::VSeparator vs_sep1;
	Gtk::ToolItem ti_sep2;
	Gtk::VSeparator vs_sep2;
	Gtk::ToolItem ti_sep3;
	Gtk::VSeparator vs_sep3;
	Gtk::ToolItem ti_sep4;
	Gtk::VSeparator vs_sep4;
	Gtk::ToolItem ti_sep5;
	Gtk::VSeparator vs_sep5;
	
	Gtk::MenuItem miFile, miEdit, miView, miHelp, miInstallGrub;
	Gtk::ImageMenuItem miExit, miSave, miPreferences, miAbout, miStartRootSelector;
	ImageMenuItemOwnKey miReload, miAdd, miRemove, miUp, miDown, miLeft, miRight;
	Gtk::Menu subFile, subEdit, subView, subHelp;
	
	bool lock_state;

	Gtk::MessageDialog burgSwitcher, pchooserQuestionDlg;

	Gtk::TreeModel::iterator getIterByProxyPtr(void* proxyPtr) const;
	Gtk::TreeModel::iterator getIterByRulePtr(void* rulePtr, const Gtk::TreeRow* parentRow = NULL) const;
	void update_move_buttons();
	void update_remove_button();
	void saveConfig();
	void updateButtonsState();
public:
	GrublistCfgDlgGtk();
	void setEventListener(EventListener_listCfgDlg& eventListener);
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
	void appendScript(std::string const& name, bool is_active, void* proxyPtr);
	void appendEntry(std::string const& name, bool is_active, void* entryPtr, bool editable, bool is_submenu, void* parentEntry = NULL);
	void showProxyNotFoundMessage();
	std::string createNewEntriesPlaceholderString(std::string const& parentMenu = "");
	std::string createPlaintextString() const;
	
	void setProxyName(void* proxy, std::string const& name, bool isModified);
	
	void swapProxies(void* a, void* b);
	
	void setDefaultTitleStatusText(std::string const& str);
	
	void removeProxy(void* p);
	
	int showExitConfirmDialog(int type);
	void showErrorMessage(std::string const& msg, std::vector<std::string> const& values);
	void clear();
	
	std::string getRuleName(void* rule);
	void setRuleName(void* rule, std::string const& newName);
	bool getRuleState(void* rule);
	void setRuleState(void* rule, bool newState);
	bool getProxyState(void* proxy);
	void setProxyState(void* proxy, bool isActive);

	void selectRule(void* rule, bool startEdit = false);

protected:
	void setEntrySensibility(const Gtk::TreeNodeChildren& list, bool sensibility);
private:
	//event handlers
	void signal_show_root_selector();
	void signal_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
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
};

#endif
