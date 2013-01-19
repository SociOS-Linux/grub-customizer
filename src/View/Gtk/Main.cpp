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

#include "Main.h"

ImageMenuItemOwnKey::ImageMenuItemOwnKey(const Gtk::StockID& id, const Gtk::AccelKey& accel_key) : Gtk::ImageMenuItem(id){
	set_accel_key(accel_key);
}

View_Gtk_Main::View_Gtk_Main()
	: tbttAdd(Gtk::Stock::UNDELETE), tbttRemove(Gtk::Stock::DELETE), tbttUp(Gtk::Stock::GO_UP), tbttDown(Gtk::Stock::GO_DOWN),
	tbttLeft(Gtk::Stock::GO_BACK), tbttRight(Gtk::Stock::GO_FORWARD),
	tbttSave(Gtk::Stock::SAVE), tbttEditEntry(Gtk::Stock::EDIT),
	miFile(gettext("_File"), true), miExit(Gtk::Stock::QUIT), tbttReload(Gtk::Stock::REFRESH),
	tbttRevert(Gtk::Stock::REVERT_TO_SAVED), tbttCreateEntry(Gtk::Stock::NEW),
	miEdit(gettext("_Edit"), true), miView(gettext("_View"), true), miHelp(gettext("_Help"), true),
	miInstallGrub(gettext("_Install to MBR …"), true),
	miAdd(Gtk::Stock::UNDELETE, Gtk::AccelKey('+', Gdk::CONTROL_MASK)), miRemove(Gtk::Stock::DELETE, Gtk::AccelKey('-', Gdk::CONTROL_MASK)), miUp(Gtk::Stock::GO_UP, Gtk::AccelKey('u', Gdk::CONTROL_MASK)), miDown(Gtk::Stock::GO_DOWN, Gtk::AccelKey('d', Gdk::CONTROL_MASK)),
	miLeft(Gtk::Stock::GO_BACK, Gtk::AccelKey('l', Gdk::CONTROL_MASK)), miRight(Gtk::Stock::GO_FORWARD, Gtk::AccelKey('r', Gdk::CONTROL_MASK)),
	miEditEntry(Gtk::Stock::EDIT, Gtk::AccelKey('e', Gdk::CONTROL_MASK)),
	miCRemove(Gtk::Stock::DELETE), miCUp(Gtk::Stock::GO_UP), miCDown(Gtk::Stock::GO_DOWN),
	miCLeft(Gtk::Stock::GO_BACK), miCRight(Gtk::Stock::GO_FORWARD), miCRename(Gtk::Stock::EDIT), miCEditEntry(Gtk::Stock::EDIT),
	miReload(Gtk::Stock::REFRESH, Gtk::AccelKey("F5")), miSave(Gtk::Stock::SAVE),
	miAbout(Gtk::Stock::ABOUT), miModifyEnvironment(Gtk::Stock::OPEN), miRevert(Gtk::Stock::REVERT_TO_SAVED),
	miCreateEntry(Gtk::Stock::NEW),
	lock_state(~0), burgSwitcher(gettext("BURG found!"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO),
	bttAdvancedSettings1(gettext("advanced settings")), bttAdvancedSettings2(gettext("advanced settings")),
	bbxAdvancedSettings1(Gtk::BUTTONBOX_END), bbxAdvancedSettings2(Gtk::BUTTONBOX_END),
	lblReloadRequired(gettext("The modifications you've done affects the visible entries. Please reload!"), Gtk::ALIGN_LEFT)
{
	win.set_icon_name("grub-customizer");

	win.set_default_size(800,600);
	win.add(vbMainSplit);
	
	vbMainSplit.pack_start(menu, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(toolbar, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(notebook);
	vbMainSplit.pack_start(statusbar, Gtk::PACK_SHRINK);

	notebook.append_page(vbEntryList, gettext("_List configuration"), true);
	vbEntryList.pack_start(infoReloadRequired, Gtk::PACK_SHRINK);
	vbEntryList.pack_start(scrEntryList);
	scrEntryList.add(tvConfList);
	statusbar.add(progressBar);
	
	scrEntryList.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrEntryList.set_shadow_type(Gtk::SHADOW_IN);
	
	infoReloadRequired.set_message_type(Gtk::MESSAGE_WARNING);
	infoReloadRequired.add_button(Gtk::Stock::REFRESH, Gtk::RESPONSE_APPLY);
	dynamic_cast<Gtk::Container&>(*infoReloadRequired.get_content_area()).add(lblReloadRequired);
	infoReloadRequired.set_no_show_all(true);

	progressBar.set_pulse_step(0.1);
	
	//toolbar
	toolbar.append(tbttSave);
	tbttSave.set_is_important(true);
	
	ti_sep1.add(vs_sep1);
	toolbar.append(ti_sep1);

	toolbar.append(tbttRemove);
	tbttRemove.set_tooltip_text(gettext("Remove selected entries (you can restore them from trash)"));
	tbttRemove.set_is_important(true);
	toolbar.append(tbttAdd);
	tbttAdd.set_tooltip_text(gettext("restore entries from trash"));
	tbttAdd.set_is_important(true);
	tbttAdd.set_label(gettext("Trash"));
	
	toolbar.append(tbttEditEntry);
	toolbar.append(tbttCreateEntry);

	ti_sep2.add(vs_sep2);
	toolbar.append(ti_sep2);
	
	toolbar.append(tbttUp);
	tbttUp.set_tooltip_text(gettext("Move up the selected entry or script"));
	toolbar.append(tbttDown);
	tbttDown.set_tooltip_text(gettext("Move down the selected entry or script"));

	ti_sep3.add(vs_sep3);
	toolbar.append(ti_sep3);

	toolbar.append(tbttLeft);
	tbttLeft.set_tooltip_text(gettext("remove this entry from the current submenu"));

	toolbar.append(tbttRight);
	tbttRight.set_tooltip_text(gettext("add this entry to a new submenu"));

	ti_sep4.add(vs_sep4);
	toolbar.append(ti_sep4);
	
	toolbar.append(tbttReload);

	toolbar.append(ti_sep5);

	ti_sep5.set_expand(true);
	ti_sep5.set_draw(false);

	toolbar.append(tbttRevert);
	tbttRevert.set_is_important(true);
	tbttRevert.set_tooltip_text(gettext("Reverts the list to the default order"));

	tbttReload.set_tooltip_text(gettext("reloads the configuration. Unsaved changes will be preserved."));
	
	this->setLockState(3);
	//menu
	menu.append(miFile);
	menu.append(miEdit);
	menu.append(miView);
	menu.append(miHelp);
	menu.append(miContext);
	
	miFile.set_submenu(subFile);
	miEdit.set_submenu(subEdit);
	miView.set_submenu(subView);
	miHelp.set_submenu(subHelp);
	miContext.set_submenu(contextMenu);

	subFile.attach(miModifyEnvironment, 0,1,0,1);
	subFile.attach(miSave, 0,1,1,2);
	subFile.attach(miInstallGrub, 0,1,2,3);
	subFile.attach(miExit, 0,1,3,4);
	
	subEdit.attach(miAdd, 0,1,0,1);
	subEdit.attach(miRemove, 0,1,1,2);
	subEdit.attach(miUp, 0,1,2,3);
	subEdit.attach(miDown, 0,1,3,4);
	subEdit.attach(miLeft, 0,1,4,5);
	subEdit.attach(miRight, 0,1,5,6);
	subEdit.attach(miEditEntry, 0,1,6,7);
	subEdit.attach(miCreateEntry, 0,1,7,8);
	subEdit.attach(miRevert, 0,1,8,9);
	

	contextMenu.attach(miCRename, 0,1,0,1);
	contextMenu.attach(miCEditEntry, 0,1,1,2);
	contextMenu.attach(miCRemove, 0,1,2,3);
	contextMenu.attach(miCUp, 0,1,3,4);
	contextMenu.attach(miCDown, 0,1,4,5);
	contextMenu.attach(miCLeft, 0,1,5,6);
	contextMenu.attach(miCRight, 0,1,6,7);

	miAdd.set_label(gettext("Restore entry"));
	miCRename.set_label(gettext("Rename"));
	miUp.set_label(gettext("Move up"));
	miCUp.set_label(gettext("Move up"));
	miDown.set_label(gettext("Move down"));
	miCDown.set_label(gettext("Move down"));
	miLeft.set_label(gettext("Remove from submenu"));
	miCLeft.set_label(gettext("Remove from submenu"));
	miRight.set_label(gettext("Create submenu"));
	miCRight.set_label(gettext("Create submenu"));


	subView.attach(miReload, 0,1,0,1);
	
	subHelp.attach(miAbout, 0,1,0,1);
	
	miModifyEnvironment.set_label(gettext("_Change Environment …"));
	miModifyEnvironment.set_use_underline(true);
	
	//burg switcher
	burgSwitcher.set_skip_taskbar_hint(false);
	burgSwitcher.set_title("Grub Customizer");
	burgSwitcher.set_icon_name("grub-customizer");
	burgSwitcher.set_secondary_text(gettext("Do you want to configure BURG instead of grub2?"));
	burgSwitcher.set_default_response(Gtk::RESPONSE_YES);

	//signals
	
	tbttUp.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),-1));
	tbttDown.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),1));
	tvConfList.get_selection()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_treeview_selection_changed));
	tvConfList.textRenderer.signal_editing_started().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_edit_name));
	tvConfList.textRenderer.signal_edited().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_edit_name_finished));
	tvConfList.signal_button_press_event().connect_notify(sigc::mem_fun(this, &View_Gtk_Main::signal_button_press));
	tvConfList.signal_popup_menu().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_popup));
	tvConfList.signal_key_press_event().connect_notify(sigc::mem_fun(this, &View_Gtk_Main::signal_key_press));
	tbttSave.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::saveConfig));
	tbttAdd.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_add_click));
	tbttRemove.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_remove_click));
	tbttLeft.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_left_click));
	tbttRight.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_right_click));
	tbttReload.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_reload_click));
	tbttEditEntry.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_edit_click));
	tbttCreateEntry.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_create_click));
	tbttRevert.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_revert));
	bttAdvancedSettings1.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_preference_click));
	bttAdvancedSettings2.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_preference_click));
	
	miUp.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),-1));
	miCUp.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),-1));
	miDown.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),1));
	miCDown.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),1));
	miLeft.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_left_click));
	miCLeft.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_left_click));
	miRight.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_right_click));
	miCRight.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_right_click));
	miSave.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::saveConfig));
	miAdd.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_add_click));
	miEditEntry.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_edit_click));
	miCEditEntry.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_edit_click));
	miCreateEntry.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_create_click));
	miRemove.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_remove_click));
	miCRemove.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_remove_click));
	miCRename.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_rename_click));
	miReload.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_reload_click));
	miInstallGrub.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_show_grub_install_dialog_click));
	miModifyEnvironment.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_show_envEditor));
	miRevert.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_revert));

	miExit.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_quit_click));
	miAbout.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_info_click));
	
	burgSwitcher.signal_response().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_burg_switcher_response));

	infoReloadRequired.signal_response().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_reload_recommendation_response));

	win.signal_delete_event().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_delete_event));

	notebook.signal_switch_page().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_tab_changed));
}

void View_Gtk_Main::setEventListener(MainController& eventListener) {
	this->eventListener = &eventListener;
}

void View_Gtk_Main::putSettingsDialog(Gtk::VBox& commonSettingsPane, Gtk::VBox& appearanceSettingsPane) {
//	notebook.append_page(this->settingsHBox, "_settings", true);
	commonSettingsPane.set_border_width(20);
	notebook.append_page(commonSettingsPane, gettext("_General settings"), true);
	commonSettingsPane.pack_end(bbxAdvancedSettings1, false, false);
	bbxAdvancedSettings1.pack_end(bttAdvancedSettings1);

	appearanceSettingsPane.set_border_width(20);
	notebook.append_page(appearanceSettingsPane, gettext("_Appearance settings"), true);
	appearanceSettingsPane.pack_end(bbxAdvancedSettings2, false, false);
	bbxAdvancedSettings2.pack_end(bttAdvancedSettings2);
}

void View_Gtk_Main::signal_edit_name(Gtk::CellEditable* editable, const Glib::ustring& path) {
	Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(path);
	Glib::ustring name = (*iter)[this->tvConfList.treeModel.name];
	editable->set_property<Glib::ustring>("text", name);
}

void View_Gtk_Main::setIsBurgMode(bool isBurgMode){
	if (isBurgMode)
		win.set_title("Grub Customizer (" + Glib::ustring(gettext("BURG Mode")) + ")");
	else
		win.set_title("Grub Customizer");

	tbttSave.set_tooltip_text(Glib::ustring(gettext("Save configuration and generate a new "))+(isBurgMode?"burg.cfg":"grub.cfg"));
}

void View_Gtk_Main::signal_button_press(GdkEventButton *event) {
	if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
		contextMenu.popup(event->button, event->time);
	}
}

bool View_Gtk_Main::signal_popup() {
	contextMenu.popup(0, gdk_event_get_time(NULL));
	return true;
}

void View_Gtk_Main::signal_key_press(GdkEventKey* key) {
	if (key->keyval == GDK_Delete) {
		this->eventListener->removeRulesAction(this->getSelectedRules());
	}
}

void View_Gtk_Main::signal_revert() {
	Gtk::MessageDialog msgDlg(gettext("Are you sure?"), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK_CANCEL);
	msgDlg.set_secondary_text(gettext("This removes all your list modifications of the bootloader menu!"));
	int response = msgDlg.run();
	if (response == Gtk::RESPONSE_OK) {
		this->eventListener->revertAction();
	}
}

void View_Gtk_Main::signal_reload_recommendation_response(int response_id) {
	if (response_id == Gtk::RESPONSE_APPLY) {
		this->eventListener->reloadAction();
	}
}

void View_Gtk_Main::signal_tab_changed(GtkNotebookPage* page, guint page_num) {
	if (this->eventListener && this->lock_state == 0) { // this->eventListener must be called because this event may be propagated from bootstrap
		this->eventListener->refreshTabAction(page_num);
	}
}

void View_Gtk_Main::showBurgSwitcher(){
	burgSwitcher.show();
}

void View_Gtk_Main::hideBurgSwitcher(){
	burgSwitcher.hide();
}

bool View_Gtk_Main::isVisible(){
	return win.is_visible();
}

void View_Gtk_Main::show(){
	win.show_all();
}

void View_Gtk_Main::hide() {
	win.hide();
}

void View_Gtk_Main::run(){
	this->show();
	Gtk::Main::run(win);
}

void View_Gtk_Main::setProgress(double progress){
	progressBar.set_fraction(progress);
	progressBar.show();
	statusbar.show();
}

void View_Gtk_Main::progress_pulse(){
	progressBar.pulse();
	progressBar.show();
	statusbar.show();
}

void View_Gtk_Main::hideProgressBar(){
	statusbar.hide();
}

void View_Gtk_Main::setStatusText(std::string const& new_status_text){
	statusbar.push(new_status_text);
}

void View_Gtk_Main::setStatusText(std::string const& name, int pos, int max){
	if (name == "") {
		statusbar.push(gettext("loading configuration…"));
	} else {
		statusbar.push(Glib::ustring::compose(gettext("loading script %2/%3 (%1)"), name, pos, max));
	}
}

void View_Gtk_Main::appendEntry(std::string const& name, void* entryPtr, bool is_placeholder, bool is_submenu, std::string const& scriptName, std::string const& defaultName, bool isEditable, bool isModified, std::map<std::string, std::string> const& options, void* parentEntry){
	Gtk::TreeIter entryRow;
	if (parentEntry) {
		entryRow = tvConfList.refTreeStore->append(this->getIterByRulePtr(parentEntry)->children());
	} else {
		entryRow = tvConfList.refTreeStore->append();
	}

	Glib::RefPtr<Gdk::Pixbuf> icon;
	std::string outputName = escapeXml(name);
	if (!is_placeholder) {
		outputName = "<b>" + outputName + "</b>";
	}
	outputName += "\n<small>";
	if (is_submenu) {
		outputName += gettext("submenu");
		icon = this->win.render_icon(Gtk::Stock::DIRECTORY, Gtk::ICON_SIZE_LARGE_TOOLBAR);
	} else if (is_placeholder) {
		outputName += gettext("placeholder");
		icon = this->win.render_icon(Gtk::Stock::FIND, Gtk::ICON_SIZE_LARGE_TOOLBAR);
	} else {
		outputName += gettext("menuentry");
		icon = this->win.render_icon(Gtk::Stock::EXECUTE, Gtk::ICON_SIZE_LARGE_TOOLBAR);
	}
	if (scriptName != "") {
		outputName += std::string(" / ") + gettext("script: ") + escapeXml(scriptName);
	}

	if (defaultName != "" && name != defaultName) {
		outputName += std::string(" / ") + gettext("default name: ") + escapeXml(defaultName);
	}

	if (options.find("_deviceName") != options.end()) {
		outputName += escapeXml(Glib::ustring(" / ") + gettext("Partition: ") + options.at("_deviceName"));
	}

	outputName += "</small>";

	if (isModified) {
		outputName = "<i>" + outputName + "</i>";
	}

	(*entryRow)[tvConfList.treeModel.name] = name;
	(*entryRow)[tvConfList.treeModel.text] = outputName;
	(*entryRow)[tvConfList.treeModel.relatedRule] = (void*)entryPtr;
	(*entryRow)[tvConfList.treeModel.is_renamable] = !is_placeholder;
	(*entryRow)[tvConfList.treeModel.is_editable] = isEditable;
	(*entryRow)[tvConfList.treeModel.is_sensitive] = !is_placeholder;
	(*entryRow)[tvConfList.treeModel.icon] = icon;


	tvConfList.expand_all();
}



void View_Gtk_Main::showProxyNotFoundMessage(){
	Gtk::MessageDialog msg(gettext("Proxy binary not found!"), false, Gtk::MESSAGE_WARNING);
	msg.set_secondary_text(gettext("You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly."));
	msg.run();
}

std::string View_Gtk_Main::createNewEntriesPlaceholderString(std::string const& parentMenu, std::string const& sourceScriptName) {
	if (sourceScriptName != "" && parentMenu != "") {
		return Glib::ustring::compose(gettext("(incoming Entries of %1, Script: %2)"), parentMenu, sourceScriptName);
	} else if (parentMenu != "") {
		return Glib::ustring::compose(gettext("(incoming Entries of %1)"), parentMenu);
	} else if (sourceScriptName != "") {
		return Glib::ustring::compose(gettext("(incoming Entries of Script: %1)"), sourceScriptName);
	} else {
		return gettext("(incoming Entries)");
	}
}

std::string View_Gtk_Main::createPlaintextString(std::string const& scriptName) const {
	if (scriptName == "") {
		return gettext("(script code)");
	} else {
		return Glib::ustring::compose(gettext("(script code of %1)"), scriptName);
	}
}

void View_Gtk_Main::saveConfig(){
	eventListener->saveAction();
}


void View_Gtk_Main::setLockState(int state){
	//state(bin):
	//	0: lock nothing
	//	1: lock normal items/buttons
	//	2: lock grub-install
	//	4: lock partition chooser
	//  8: lock settings dialog
	tbttSave.set_sensitive((state & 1) == 0);
	miSave.set_sensitive((state & 1) == 0);

	tbttUp.set_sensitive((state & 1) == 0);
	miUp.set_sensitive((state & 1) == 0);
	miCUp.set_sensitive((state & 1) == 0);
	tbttDown.set_sensitive((state & 1) == 0);
	miDown.set_sensitive((state & 1) == 0);
	miCDown.set_sensitive((state & 1) == 0);
	tbttLeft.set_sensitive((state & 1) == 0);
	miLeft.set_sensitive((state & 1) == 0);
	miCLeft.set_sensitive((state & 1) == 0);
	tbttRight.set_sensitive((state & 1) == 0);
	miRight.set_sensitive((state & 1) == 0);
	miCRight.set_sensitive((state & 1) == 0);
	tbttEditEntry.set_sensitive((state & 1) == 0);
	miEditEntry.set_sensitive((state & 1) == 0);
	miCEditEntry.set_sensitive((state & 1) == 0);

	tbttAdd.set_sensitive((state & 1) == 0);
	miAdd.set_sensitive((state & 1) == 0);
	tbttRemove.set_sensitive((state & 1) == 0);
	miRemove.set_sensitive((state & 1) == 0);
	miCRemove.set_sensitive((state & 1) == 0);

	miCRename.set_sensitive((state & 1) == 0);
	
	tbttReload.set_sensitive((state & 1) == 0);
	miReload.set_sensitive((state & 1) == 0);

	tbttRevert.set_sensitive((state & 1) == 0);
	miRevert.set_sensitive((state & 1) == 0);

	miModifyEnvironment.set_sensitive((state & 4) == 0);
	bttAdvancedSettings1.set_sensitive((state & 8) == 0);
	bttAdvancedSettings1.set_sensitive((state & 8) == 0);

	tvConfList.set_sensitive((state & 1) == 0);
	

	miCreateEntry.set_sensitive((state & 1) == 0);
	tbttCreateEntry.set_sensitive((state & 1) == 0);

	miInstallGrub.set_sensitive((state & 2) == 0);
	
	if ((state & 1) == 0) {
		this->updateButtonsState();
	}

	this->lock_state = state;
}


void View_Gtk_Main::updateButtonsState(){
	update_remove_button();
	update_move_buttons();

	std::vector<Gtk::TreeModel::Path> selectedElementents = this->tvConfList.get_selection()->get_selected_rows();
	bool renamableEntrySelected = selectedElementents.size() == 1 && (*this->tvConfList.refTreeStore->get_iter(selectedElementents[0]))[this->tvConfList.treeModel.is_renamable];
	bool editableEntrySelected = selectedElementents.size() == 1 && (*this->tvConfList.refTreeStore->get_iter(selectedElementents[0]))[this->tvConfList.treeModel.is_editable];
	miCRename.set_sensitive(renamableEntrySelected);
	tbttEditEntry.set_sensitive(editableEntrySelected);
	miEditEntry.set_sensitive(editableEntrySelected);
	miCEditEntry.set_sensitive(editableEntrySelected);
}

bool View_Gtk_Main::selectedEntriesAreOnSameLevel() {
	assert(this->tvConfList.get_selection()->count_selected_rows() >= 1);

	std::vector<Gtk::TreeModel::Path> pathes = this->tvConfList.get_selection()->get_selected_rows();
	bool result = true;
	if (!this->tvConfList.refTreeStore->get_iter(pathes[0])->parent()) { // first entry is on toplevel, so all entries should be there
		for (std::vector<Gtk::TreeModel::Path>::iterator pathIter = pathes.begin(); pathIter != pathes.end(); pathIter++) {
			if (this->tvConfList.refTreeStore->get_iter(*pathIter)->parent()) {
				result = false;
				break;
			}
		}
	} else {
		Gtk::TreeModel::Path parent = this->tvConfList.refTreeStore->get_path(this->tvConfList.refTreeStore->get_iter(pathes[0])->parent());
		for (std::vector<Gtk::TreeModel::Path>::iterator pathIter = pathes.begin(); pathIter != pathes.end(); pathIter++) {
			Gtk::TreeModel::iterator treeIter = this->tvConfList.refTreeStore->get_iter(*pathIter);
			if (!treeIter->parent() || parent != this->tvConfList.refTreeStore->get_path(treeIter->parent())) {
				result = false;
				break;
			}
		}
	}
	return result;
}

bool View_Gtk_Main::selectedEntriesAreSubsequent() {
	assert(this->selectedEntriesAreOnSameLevel());
	if (this->tvConfList.get_selection()->count_selected_rows() == 1) {
		return true;
	}
	std::vector<Gtk::TreeModel::Path> pathes = this->tvConfList.get_selection()->get_selected_rows();

	Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(pathes[0]);

	std::vector<Gtk::TreeModel::Path>::iterator pathIter = pathes.begin();
	pathIter++;
	for (; pathIter != pathes.end(); pathIter++) {
		iter++;
		if (iter != this->tvConfList.refTreeStore->get_iter(*pathIter)) {
			return false;
		}
	}
	return true;
}

std::list<void*> View_Gtk_Main::getSelectedRules() {
	std::list<void*> rules;
	std::vector<Gtk::TreeModel::Path> pathes = tvConfList.get_selection()->get_selected_rows();
	for (std::vector<Gtk::TreeModel::Path>::iterator iter = pathes.begin(); iter != pathes.end(); iter++) {
		void* rptr = (*tvConfList.refTreeStore->get_iter(*iter))[tvConfList.treeModel.relatedRule];
		rules.push_back(rptr);
	}

	return rules;
}


void View_Gtk_Main::signal_reload_click(){
	eventListener->reloadAction();
}

Gtk::TreeModel::iterator View_Gtk_Main::getIterByRulePtr(void* rulePtr, const Gtk::TreeRow* parentRow) const {
	const Gtk::TreeNodeChildren children = parentRow ? parentRow->children() : tvConfList.refTreeStore->children();
	for (Gtk::TreeModel::const_iterator iter = children.begin(); iter != children.end(); iter++) {
		if ((*iter)[tvConfList.treeModel.relatedRule] == rulePtr)
			return iter;
		try {
			return this->getIterByRulePtr(rulePtr, &**iter); //recursively search for the treeview item
		} catch (ItemNotFoundException const& e) {
			//(ignore ItemNotFoundException exception)
		}
	}
	throw ItemNotFoundException("rule not found", __FILE__, __LINE__);
}

void View_Gtk_Main::signal_edit_name_finished(const Glib::ustring& path, const Glib::ustring& new_text){
	if (this->lock_state == 0){
		Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(path);
		eventListener->renameRuleAction((void*)(*iter)[tvConfList.treeModel.relatedRule], new_text);
	}
}

void View_Gtk_Main::signal_show_envEditor(){
	eventListener->showEnvEditorAction();
}



std::string View_Gtk_Main::getRuleName(void* rule){
	Gtk::TreeModel::iterator iter = this->getIterByRulePtr(rule);
	return (Glib::ustring)(*iter)[tvConfList.treeModel.text];
}
void View_Gtk_Main::setRuleName(void* rule, std::string const& newName){
	Gtk::TreeModel::iterator iter = this->getIterByRulePtr(rule);
	this->setLockState(~0);
	(*iter)[tvConfList.treeModel.name] = newName;
	this->setLockState(0);
}


void View_Gtk_Main::selectRule(void* rule, bool startEdit) {
	try {
		this->tvConfList.get_selection()->select(this->getIterByRulePtr(rule));
		if (startEdit) {
			this->tvConfList.set_cursor(this->tvConfList.refTreeStore->get_path(this->getIterByRulePtr(rule)), *this->tvConfList.get_column(0), true);
		}
	} catch (ItemNotFoundException const& e) {
		// do nothing
	}
}

void View_Gtk_Main::selectRules(std::list<void*> rules) {
	for (std::list<void*>::iterator iter = rules.begin(); iter != rules.end(); iter++) {
		this->tvConfList.get_selection()->select(this->getIterByRulePtr(*iter));
	}
}

void View_Gtk_Main::setTrashCounter(int count) {
	this->tbttAdd.set_label(Glib::ustring::compose(gettext("Trash (%1)"), count));
}

void View_Gtk_Main::showReloadRecommendation() {
	this->infoReloadRequired.show();
	this->lblReloadRequired.show();
}

void View_Gtk_Main::hideReloadRecommendation() {
	this->infoReloadRequired.hide();
}

void View_Gtk_Main::showPlaintextRemoveWarning() {
	Gtk::MessageDialog dlg(gettext("Removing Script Code can cause problems when trying to boot entries relying on it. Are you sure you want to do it anyway?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
	int result = dlg.run();
	if (result == Gtk::RESPONSE_YES) {
		eventListener->removeRulesAction(this->getSelectedRules(), true);
	}
}

void View_Gtk_Main::showSystemRuleRemoveWarning() {
	Gtk::MessageDialog dlg(gettext("You're trying to remove an entry of the currently running system. Make sure there are other working entries of this system!\nIf you just want to remove old kernels: The better way is uninstalling them instead of just hiding them in boot menu."), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
	int result = dlg.run();
	if (result == Gtk::RESPONSE_OK) {
		eventListener->removeRulesAction(this->getSelectedRules(), true);
	}
}

void View_Gtk_Main::signal_move_click(int direction){
	if (this->lock_state == 0){
		assert(direction == 1 || direction == -1);

		//if rule swap
		eventListener->moveAction(this->getSelectedRules(), direction);
	}
}

void View_Gtk_Main::update_remove_button(){
	if (tvConfList.get_selection()->count_selected_rows() >= 1){
		tbttRemove.set_sensitive(true);
		miRemove.set_sensitive(true);
		miCRemove.set_sensitive(true);
	} else {
		tbttRemove.set_sensitive(false);
		miRemove.set_sensitive(false);
		miCRemove.set_sensitive(false);
	}
}

void View_Gtk_Main::signal_treeview_selection_changed(){
	if (this->lock_state == 0){
		this->updateButtonsState();
	}
}

void View_Gtk_Main::signal_entry_edit_click() {
	std::list<void*> rules = this->getSelectedRules();
	assert(rules.size() == 1);
	eventListener->showEntryEditorAction(rules.front());
}

void View_Gtk_Main::signal_entry_create_click() {
	eventListener->showEntryCreatorAction();
}

void View_Gtk_Main::signal_add_click(){
	eventListener->showTrashAction();
}

void View_Gtk_Main::signal_remove_click() {
	eventListener->removeRulesAction(this->getSelectedRules());
}

void View_Gtk_Main::signal_rename_click() {
	this->selectRule(this->getSelectedRules().front(), true);
}

void View_Gtk_Main::signal_preference_click(){
	eventListener->showSettingsAction();
}

void View_Gtk_Main::update_move_buttons(){
	int selectedRowsCount = tvConfList.get_selection()->count_selected_rows();
	bool is_toplevel = false;
	bool sameLevel = false;
	bool subsequent = false;

	if (selectedRowsCount >= 1) {
		sameLevel = this->selectedEntriesAreOnSameLevel();
		if (sameLevel) {
			subsequent = this->selectedEntriesAreSubsequent();
		}
	}

	if (selectedRowsCount >= 1 && sameLevel) {
		std::vector<Gtk::TreeModel::Path> pathes = tvConfList.get_selection()->get_selected_rows();
		Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(pathes[0]);
		is_toplevel = iter->parent() ? false : true;
	}

	tbttUp.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miUp.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miCUp.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	tbttDown.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miDown.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miCDown.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	tbttLeft.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !is_toplevel); //selected entry must be inside a submenu
	miLeft.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !is_toplevel); //selected entry must be inside a submenu
	miCLeft.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !is_toplevel); //selected entry must be inside a submenu
	tbttRight.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miRight.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miCRight.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
}


void View_Gtk_Main::close(){
	win.hide();
	Gtk::Main::quit();
}

/**
 * @param type int: which type of dialog to show (1: changes unsaved, 2: conf not up to date, 3: 1 + 2)
 * @return int: type of the answer: 0: cancel, 1: yes, 2: no
 */
int View_Gtk_Main::showExitConfirmDialog(int type){
	int dlgResponse = Gtk::RESPONSE_NO;
	if (type != 0){
		Gtk::MessageDialog msgDlg("", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_NONE);
		if (type & 2){
			msgDlg.set_message(gettext("The saved configuration is not up to date!"));
			msgDlg.set_secondary_text(gettext("The generated configuration didn't equal to the saved configuration on startup. So what you see now may not be what you see when you restart your pc. To fix this, click update!"));
			
			Gtk::Button* btnQuit = msgDlg.add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_NO);
			btnQuit->set_label(gettext("_Quit without update"));
			msgDlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			Gtk::Button* btnSave = msgDlg.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_YES);
			btnSave->set_label(gettext("_Update & Quit"));
		}
		if ((type & 1) && !(type & 2)){
			msgDlg.property_message_type() = Gtk::MESSAGE_QUESTION;
			msgDlg.set_message(gettext("Do you want to save your modifications?"));

			Gtk::Button* btnQuit = msgDlg.add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_NO);
			btnQuit->set_label(gettext("_Quit without saving"));
			msgDlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			Gtk::Button* btnSave = msgDlg.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_YES);
			btnSave->set_label(gettext("_Save & Quit"));
		}
		if ((type & 3) == 3){
			msgDlg.set_secondary_text(msgDlg.property_secondary_text()+"\n\n"+gettext("AND: your modifications are still unsaved, update will save them too!"));
		}

		msgDlg.set_default_response(Gtk::RESPONSE_YES);

		dlgResponse = msgDlg.run();
	}
	switch (dlgResponse){
		case Gtk::RESPONSE_CANCEL: return 0;
		case Gtk::RESPONSE_YES: return 1;
		default: return 2;
	}
}

bool View_Gtk_Main::signal_delete_event(GdkEventAny* event){ //return value: keep window open
	eventListener->exitAction();
	return true;
}

void View_Gtk_Main::signal_quit_click(){
	eventListener->exitAction();
}


void View_Gtk_Main::signal_show_grub_install_dialog_click(){
	eventListener->showInstallerAction();
}

void View_Gtk_Main::signal_move_left_click() {
	eventListener->removeSubmenuAction(this->getSelectedRules());
}

void View_Gtk_Main::signal_move_right_click() {
	eventListener->createSubmenuAction(this->getSelectedRules());
}

void View_Gtk_Main::showErrorMessage(std::string const& msg, std::vector<std::string> const& values = std::vector<std::string>()){
	Glib::ustring msg2 = msg;
	switch (values.size()) {
	case 1:	msg2 = Glib::ustring::compose(msg, values[0]); break;
	case 2:	msg2 = Glib::ustring::compose(msg, values[0], values[1]); break;
	case 3:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2]); break;
	case 4:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3]); break;
	case 5:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4]); break;
	case 6:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4], values[5]); break;
	case 7:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4], values[5], values[6]); break;
	case 8:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7]); break;
	case 9: msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8]); break;
	}
	Gtk::MessageDialog(msg2, false, Gtk::MESSAGE_ERROR).run();
}

bool View_Gtk_Main::askForEnvironmentSettings(std::string const& failedCmd, std::string const& errorMessage) {
	Glib::ustring msg = Glib::ustring::compose(gettext("%1 couldn't be executed successfully. error message:\n %2"), failedCmd, errorMessage);
	Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_NONE);
	dlg.add_button(gettext("Environment settings"), Gtk::RESPONSE_YES);
	dlg.add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_CLOSE);
	dlg.set_default_response(Gtk::RESPONSE_CLOSE);
	int response = dlg.run();
	return response == Gtk::RESPONSE_YES;
}

void View_Gtk_Main::clear(){
	tvConfList.refTreeStore->clear();
}

bool View_Gtk_Main::confirmUnsavedSwitch() {
	Gtk::MessageDialog dlg(gettext("Do you want to proceed without saving the current configuration?"), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
	dlg.set_title(gettext("There are unsaved modifications!"));
	dlg.set_default_response(Gtk::RESPONSE_YES);
	int response = dlg.run();
	return response == Gtk::RESPONSE_YES;
}

void View_Gtk_Main::signal_info_click(){
	eventListener->showAboutAction();
}

void View_Gtk_Main::signal_burg_switcher_response(int response_id){
	if (response_id == Gtk::RESPONSE_DELETE_EVENT)
		eventListener->cancelBurgSwitcherAction();
	else
		eventListener->initModeAction(response_id == Gtk::RESPONSE_YES);
}

View_Gtk_Main_List::View_Gtk_Main_List(){
	refTreeStore = Gtk::TreeStore::create(treeModel);
	this->set_model(refTreeStore);

	this->append_column(this->mainColumn);
	this->mainColumn.pack_start(pixbufRenderer, false);
	this->mainColumn.add_attribute(pixbufRenderer.property_pixbuf(), treeModel.icon);
	this->mainColumn.pack_start(this->textRenderer, true);
	this->mainColumn.add_attribute(this->textRenderer.property_markup(), treeModel.text);
	this->mainColumn.add_attribute(this->textRenderer.property_editable(), treeModel.is_renamable);
	this->mainColumn.set_spacing(10);

	this->set_headers_visible(false);
	this->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	this->set_rubber_banding(true);
}

View_Gtk_Main_List::TreeModel::TreeModel(){
	this->add(name);
	this->add(text);
	this->add(relatedRule);
	this->add(is_other_entries_marker);
	this->add(is_renamable);
	this->add(is_editable);
	this->add(is_sensitive);
	this->add(icon);
}
