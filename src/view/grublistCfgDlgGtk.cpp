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

#include "grublistCfgDlgGtk.h"

ImageMenuItemOwnKey::ImageMenuItemOwnKey(const Gtk::StockID& id, const Gtk::AccelKey& accel_key) : Gtk::ImageMenuItem(id){
	set_accel_key(accel_key);
}

GrublistCfgDlgGtk::GrublistCfgDlgGtk()
	: tbttAdd(Gtk::Stock::UNDELETE), tbttRemove(Gtk::Stock::DELETE), tbttUp(Gtk::Stock::GO_UP), tbttDown(Gtk::Stock::GO_DOWN),
	tbttLeft(Gtk::Stock::GO_BACK), tbttRight(Gtk::Stock::GO_FORWARD),
	tbttSave(Gtk::Stock::SAVE),
	miFile(gettext("_File"), true), miExit(Gtk::Stock::QUIT), tbttReload(Gtk::Stock::REFRESH),
	miEdit(gettext("_Edit"), true), miView(gettext("_View"), true), miHelp(gettext("_Help"), true),
	miInstallGrub(gettext("_Install to MBR …"), true),
	miAdd(Gtk::Stock::UNDELETE, Gtk::AccelKey('+', Gdk::CONTROL_MASK)), miRemove(Gtk::Stock::DELETE, Gtk::AccelKey('-', Gdk::CONTROL_MASK)), miUp(Gtk::Stock::GO_UP, Gtk::AccelKey('u', Gdk::CONTROL_MASK)), miDown(Gtk::Stock::GO_DOWN, Gtk::AccelKey('d', Gdk::CONTROL_MASK)),
	miLeft(Gtk::Stock::GO_BACK, Gtk::AccelKey('l', Gdk::CONTROL_MASK)), miRight(Gtk::Stock::GO_FORWARD, Gtk::AccelKey('r', Gdk::CONTROL_MASK)),
	miReload(Gtk::Stock::REFRESH, Gtk::AccelKey("F5")), miSave(Gtk::Stock::SAVE),
	miAbout(Gtk::Stock::ABOUT), miStartRootSelector(Gtk::Stock::OPEN),
	lock_state(~0), burgSwitcher(gettext("BURG found!"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO),
	pchooserQuestionDlg(gettext("No Bootloader found"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO),
	bttAdvancedSettings1(gettext("advanced settings")), bttAdvancedSettings2(gettext("advanced settings")),
	bbxAdvancedSettings1(Gtk::BUTTONBOX_END), bbxAdvancedSettings2(Gtk::BUTTONBOX_END)
{
	win.set_icon_name("grub-customizer");

	win.set_default_size(800,600);
	win.add(vbMainSplit);
	
	vbMainSplit.pack_start(menu, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(toolbar, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(notebook);
	vbMainSplit.pack_start(statusbar, Gtk::PACK_SHRINK);

	notebook.append_page(scrEntryList, gettext("_list configuration"), true);
	scrEntryList.add(tvConfList);
	statusbar.add(progressBar);
	
	scrEntryList.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrEntryList.set_shadow_type(Gtk::SHADOW_IN);
	
	progressBar.set_pulse_step(0.1);
	
	//toolbar
	toolbar.append(tbttSave);
	tbttSave.set_is_important(true);
	
	ti_sep1.add(vs_sep1);
	toolbar.append(ti_sep1);

	toolbar.append(tbttRemove);
	tbttRemove.set_tooltip_text(gettext("Remove selected entries"));
	tbttRemove.set_is_important(true);
	toolbar.append(tbttAdd);
	tbttAdd.set_tooltip_text(gettext("restore entries from trash"));
	tbttAdd.set_is_important(true);
	tbttAdd.set_label(gettext("Trash"));
	
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

	tbttReload.set_tooltip_text(gettext("reload configuration"));
	
	this->setLockState(3);
	//menu
	menu.append(miFile);
	menu.append(miEdit);
	menu.append(miView);
	menu.append(miHelp);
	
	miFile.set_submenu(subFile);
	miEdit.set_submenu(subEdit);
	miView.set_submenu(subView);
	miHelp.set_submenu(subHelp);
	
	subFile.attach(miStartRootSelector, 0,1,0,1);
	subFile.attach(miSave, 0,1,1,2);
	subFile.attach(miInstallGrub, 0,1,2,3);
	subFile.attach(miExit, 0,1,3,4);
	
	subEdit.attach(miAdd, 0,1,0,1);
	subEdit.attach(miRemove, 0,1,1,2);
	subEdit.attach(miUp, 0,1,2,3);
	subEdit.attach(miDown, 0,1,3,4);
	subEdit.attach(miLeft, 0,1,4,5);
	subEdit.attach(miRight, 0,1,5,6);
	
	subView.attach(miReload, 0,1,0,1);
	
	subHelp.attach(miAbout, 0,1,0,1);
	
	miStartRootSelector.set_label(gettext("Select _partition …"));
	miStartRootSelector.set_use_underline(true);
	
	//burg switcher
	burgSwitcher.set_skip_taskbar_hint(false);
	burgSwitcher.set_title("Grub Customizer");
	burgSwitcher.set_icon_name("grub-customizer");
	burgSwitcher.set_secondary_text(gettext("Do you want to configure BURG instead of grub2?"));
	burgSwitcher.set_default_response(Gtk::RESPONSE_YES);

	//partition chooser question
	pchooserQuestionDlg.set_skip_taskbar_hint(false);
	pchooserQuestionDlg.set_title("Grub Customizer");
	pchooserQuestionDlg.set_icon_name("grub-customizer");
	pchooserQuestionDlg.set_secondary_text(gettext("Do you want to select another root partition?"));
	pchooserQuestionDlg.set_default_response(Gtk::RESPONSE_YES);

	//signals
	
	tbttUp.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_click),-1));
	tbttDown.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_click),1));
	tvConfList.get_selection()->signal_changed().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_treeview_selection_changed));
	tvConfList.textRenderer.signal_editing_started().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_edit_name));
	tvConfList.textRenderer.signal_edited().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_edit_name_finished));
	tbttSave.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::saveConfig));
	tbttAdd.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_add_click));
	tbttRemove.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_remove_click));
	tbttLeft.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_left_click));
	tbttRight.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_right_click));
	tbttReload.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_reload_click));
	bttAdvancedSettings1.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_preference_click));
	bttAdvancedSettings2.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_preference_click));
	
	miUp.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_click),-1));
	miDown.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_click),1));
	miLeft.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_left_click));
	miRight.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_right_click));
	miSave.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::saveConfig));
	miAdd.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_add_click));
	miRemove.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_remove_click));
	miReload.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_reload_click));
	miInstallGrub.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_show_grub_install_dialog_click));
	miStartRootSelector.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_show_root_selector));

	miExit.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_quit_click));
	miAbout.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_info_click));
	
	burgSwitcher.signal_response().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_burg_switcher_response));
	pchooserQuestionDlg.signal_response().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_partition_chooser_question_response));

	win.signal_delete_event().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_delete_event));

}

void GrublistCfgDlgGtk::setEventListener(EventListener_listCfgDlg& eventListener) {
	this->eventListener = &eventListener;
}

void GrublistCfgDlgGtk::putSettingsDialog(Gtk::VBox& commonSettingsPane, Gtk::VBox& appearanceSettingsPane) {
//	notebook.append_page(this->settingsHBox, "_settings", true);
	commonSettingsPane.set_border_width(20);
	notebook.append_page(commonSettingsPane, gettext("_common settings"), true);
	commonSettingsPane.pack_end(bbxAdvancedSettings1, false, false);
	bbxAdvancedSettings1.pack_end(bttAdvancedSettings1);

	appearanceSettingsPane.set_border_width(20);
	notebook.append_page(appearanceSettingsPane, gettext("_appearance settings"), true);
	appearanceSettingsPane.pack_end(bbxAdvancedSettings2, false, false);
	bbxAdvancedSettings2.pack_end(bttAdvancedSettings2);
}

void GrublistCfgDlgGtk::signal_edit_name(Gtk::CellEditable* editable, const Glib::ustring& path) {
	Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(path);
	Glib::ustring name = (*iter)[this->tvConfList.treeModel.name];
	editable->set_property<Glib::ustring>("text", name);
}

void GrublistCfgDlgGtk::setIsBurgMode(bool isBurgMode){
	if (isBurgMode)
		win.set_title("Grub Customizer (" + Glib::ustring(gettext("BURG Mode")) + ")");
	else
		win.set_title("Grub Customizer");

	tbttSave.set_tooltip_text(Glib::ustring(gettext("Save configuration and generate a new "))+(isBurgMode?"burg.cfg":"grub.cfg"));
}


void GrublistCfgDlgGtk::showBurgSwitcher(){
	burgSwitcher.show();
}

void GrublistCfgDlgGtk::hideBurgSwitcher(){
	burgSwitcher.hide();
}

void GrublistCfgDlgGtk::showPartitionChooserQuestion(){
	pchooserQuestionDlg.show();
}
void GrublistCfgDlgGtk::hidePartitionChooserQuestion(){
	pchooserQuestionDlg.hide();
}
bool GrublistCfgDlgGtk::isVisible(){
	return win.is_visible();
}

void GrublistCfgDlgGtk::show(){
	win.show_all();
}

void GrublistCfgDlgGtk::run(){
	this->show();
	Gtk::Main::run(win);
}

void GrublistCfgDlgGtk::setProgress(double progress){
	progressBar.set_fraction(progress);
	progressBar.show();
	statusbar.show();
}

void GrublistCfgDlgGtk::progress_pulse(){
	progressBar.pulse();
	progressBar.show();
	statusbar.show();
}

void GrublistCfgDlgGtk::hideProgressBar(){
	statusbar.hide();
}

void GrublistCfgDlgGtk::setStatusText(std::string const& new_status_text){
	statusbar.push(new_status_text);
}

void GrublistCfgDlgGtk::appendEntry(std::string const& name, void* entryPtr, bool is_placeholder, bool is_submenu, std::string const& scriptName, std::string const& defaultName, void* parentEntry){
	Gtk::TreeIter entryRow;
	if (parentEntry) {
		entryRow = tvConfList.refTreeStore->append(this->getIterByRulePtr(parentEntry)->children());
	} else {
		entryRow = tvConfList.refTreeStore->append();
	}

	Glib::RefPtr<Gdk::Pixbuf> icon;
	std::string outputName = name + "\n";
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
		outputName += std::string(" / ") + gettext("script: ") + scriptName;
	}

	if (defaultName != "" && name != defaultName) {
		outputName += std::string(" / ") + gettext("default name: ") + defaultName;
	}

	(*entryRow)[tvConfList.treeModel.name] = name;
	(*entryRow)[tvConfList.treeModel.text] = outputName;
	(*entryRow)[tvConfList.treeModel.relatedRule] = (void*)entryPtr;
	(*entryRow)[tvConfList.treeModel.is_editable] = !is_placeholder;
	(*entryRow)[tvConfList.treeModel.is_sensitive] = !is_placeholder;
	(*entryRow)[tvConfList.treeModel.icon] = icon;


	tvConfList.expand_all();
}



void GrublistCfgDlgGtk::showProxyNotFoundMessage(){
	Gtk::MessageDialog msg(gettext("Proxy binary not found!"), false, Gtk::MESSAGE_WARNING);
	msg.set_secondary_text(gettext("You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly."));
	msg.run();
}

std::string GrublistCfgDlgGtk::createNewEntriesPlaceholderString(std::string const& parentMenu, std::string const& sourceScriptName) {
	if (sourceScriptName != "" && parentMenu != "") {
		return Glib::ustring::compose(gettext("(new Entries of %1, Script: %2)"), parentMenu, sourceScriptName);
	} else if (parentMenu != "") {
		return Glib::ustring::compose(gettext("(new Entries of %1)"), parentMenu);
	} else if (sourceScriptName != "") {
		return Glib::ustring::compose(gettext("(new Entries of Script: %1)"), sourceScriptName);
	} else {
		return gettext("(new Entries)");
	}
}

std::string GrublistCfgDlgGtk::createPlaintextString() const {
	return gettext("(script code)");
}

void GrublistCfgDlgGtk::saveConfig(){
	eventListener->save_request();
}


void GrublistCfgDlgGtk::setLockState(int state){
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
	tbttDown.set_sensitive((state & 1) == 0);
	miDown.set_sensitive((state & 1) == 0);
	tbttLeft.set_sensitive((state & 1) == 0);
	miLeft.set_sensitive((state & 1) == 0);
	tbttRight.set_sensitive((state & 1) == 0);
	miRight.set_sensitive((state & 1) == 0);

	tbttAdd.set_sensitive((state & 1) == 0);
	miAdd.set_sensitive((state & 1) == 0);
	tbttRemove.set_sensitive((state & 1) == 0);
	miRemove.set_sensitive((state & 1) == 0);
	
	tbttReload.set_sensitive((state & 1) == 0);
	miReload.set_sensitive((state & 1) == 0);
	miStartRootSelector.set_sensitive((state & 4) == 0);
	bttAdvancedSettings1.set_sensitive((state & 8) == 0);
	bttAdvancedSettings1.set_sensitive((state & 8) == 0);

	tvConfList.set_sensitive((state & 1) == 0);
	
	miInstallGrub.set_sensitive((state & 2) == 0);
	
	if ((state & 1) == 0) {
		this->updateButtonsState();
	}

	this->lock_state = state;
}


void GrublistCfgDlgGtk::updateButtonsState(){
	update_remove_button();
	update_move_buttons();
}

bool GrublistCfgDlgGtk::selectedEntriesAreOnSameLevel() {
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

bool GrublistCfgDlgGtk::selectedEntriesAreSubsequent() {
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

std::list<void*> GrublistCfgDlgGtk::getSelectedRules() {
	std::list<void*> rules;
	std::vector<Gtk::TreeModel::Path> pathes = tvConfList.get_selection()->get_selected_rows();
	for (std::vector<Gtk::TreeModel::Path>::iterator iter = pathes.begin(); iter != pathes.end(); iter++) {
		void* rptr = (*tvConfList.refTreeStore->get_iter(*iter))[tvConfList.treeModel.relatedRule];
		rules.push_back(rptr);
	}

	return rules;
}


void GrublistCfgDlgGtk::signal_reload_click(){
	eventListener->reload_request();
}

Gtk::TreeModel::iterator GrublistCfgDlgGtk::getIterByRulePtr(void* rulePtr, const Gtk::TreeRow* parentRow) const {
	const Gtk::TreeNodeChildren children = parentRow ? parentRow->children() : tvConfList.refTreeStore->children();
	for (Gtk::TreeModel::const_iterator iter = children.begin(); iter != children.end(); iter++) {
		if ((*iter)[tvConfList.treeModel.relatedRule] == rulePtr)
			return iter;
		try {
			return this->getIterByRulePtr(rulePtr, &**iter); //recursively search for the treeview item
		} catch (GrublistCfgDlg::Exception e) {
			if (e != RULE_ITER_NOT_FOUND)
				throw e;
			//(ignore RULE_ITER_NOT_FOUND exceptions)
		}
	}
	throw RULE_ITER_NOT_FOUND;
}

void GrublistCfgDlgGtk::signal_edit_name_finished(const Glib::ustring& path, const Glib::ustring& new_text){
	if (this->lock_state == 0){
		Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(path);
		eventListener->signal_entry_renamed((void*)(*iter)[tvConfList.treeModel.relatedRule], new_text);
	}
}

void GrublistCfgDlgGtk::signal_show_root_selector(){
	eventListener->rootSelector_request();
}



std::string GrublistCfgDlgGtk::getRuleName(void* rule){
	Gtk::TreeModel::iterator iter = this->getIterByRulePtr(rule);
	return (Glib::ustring)(*iter)[tvConfList.treeModel.text];
}
void GrublistCfgDlgGtk::setRuleName(void* rule, std::string const& newName){
	Gtk::TreeModel::iterator iter = this->getIterByRulePtr(rule);
	this->setLockState(~0);
	(*iter)[tvConfList.treeModel.name] = newName;
	this->setLockState(0);
}


void GrublistCfgDlgGtk::selectRule(void* rule, bool startEdit) {
	try {
		this->tvConfList.get_selection()->select(this->getIterByRulePtr(rule));
		if (startEdit) {
			this->tvConfList.set_cursor(this->tvConfList.refTreeStore->get_path(this->getIterByRulePtr(rule)), *this->tvConfList.get_column(0), true);
		}
	} catch (GrublistCfgDlg::Exception e) {
		if (e != RULE_ITER_NOT_FOUND)
			throw e;
	}
}

void GrublistCfgDlgGtk::selectRules(std::list<void*> rules) {
	for (std::list<void*>::iterator iter = rules.begin(); iter != rules.end(); iter++) {
		this->tvConfList.get_selection()->select(this->getIterByRulePtr(*iter));
	}
}



void GrublistCfgDlgGtk::signal_move_click(int direction){
	if (this->lock_state == 0){
		assert(direction == 1 || direction == -1);

		//if rule swap
		eventListener->ruleAdjustment_requested(this->getSelectedRules(), direction);
	}
}

void GrublistCfgDlgGtk::update_remove_button(){
	if (tvConfList.get_selection()->count_selected_rows() >= 1){
		tbttRemove.set_sensitive(true);
		miRemove.set_sensitive(true);
	} else {
		tbttRemove.set_sensitive(false);
		miRemove.set_sensitive(false);
	}
}

void GrublistCfgDlgGtk::setDefaultTitleStatusText(std::string const& str){
	this->setStatusText(gettext("Default title: ")+str);
}

void GrublistCfgDlgGtk::signal_treeview_selection_changed(){
	if (this->lock_state == 0){
		if (tvConfList.get_selection()->count_selected_rows()) {
			std::vector<Gtk::TreeModel::Path> selectedRows = tvConfList.get_selection()->get_selected_rows();
			Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(selectedRows[0]);

			void* rptr = (*iter)[tvConfList.treeModel.relatedRule];
			this->eventListener->ruleSelected(rptr);
		} else {
			this->eventListener->ruleSelected(NULL);
		}

		this->updateButtonsState();
	}
}

void GrublistCfgDlgGtk::signal_add_click(){
	eventListener->entryAddDlg_requested();
}

void GrublistCfgDlgGtk::signal_remove_click() {
	eventListener->signal_entry_remove_requested(this->getSelectedRules());
}

void GrublistCfgDlgGtk::signal_preference_click(){
	eventListener->settings_dialog_request();
}

void GrublistCfgDlgGtk::update_move_buttons(){
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
	tbttDown.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miDown.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	tbttLeft.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !is_toplevel); //selected entry must be inside a submenu
	miLeft.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !is_toplevel); //selected entry must be inside a submenu
	tbttRight.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miRight.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
}


void GrublistCfgDlgGtk::close(){
	win.hide();
	Gtk::Main::quit();
}

/**
 * @param type int: which type of dialog to show (1: changes unsaved, 2: conf not up to date, 3: 1 + 2)
 * @return int: type of the answer: 0: cancel, 1: yes, 2: no
 */
int GrublistCfgDlgGtk::showExitConfirmDialog(int type){
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
		if (type & 1 && !(type & 2)){
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

bool GrublistCfgDlgGtk::signal_delete_event(GdkEventAny* event){ //return value: keep window open
	eventListener->exitRequest();
	return true;
}

void GrublistCfgDlgGtk::signal_quit_click(){
	eventListener->exitRequest();
}


void GrublistCfgDlgGtk::signal_show_grub_install_dialog_click(){
	eventListener->installDialogRequest();
}

void GrublistCfgDlgGtk::signal_move_left_click() {
	eventListener->removeSubmenuRequest(this->getSelectedRules());
}

void GrublistCfgDlgGtk::signal_move_right_click() {
	eventListener->createSubmenuRequest(this->getSelectedRules());
}

void GrublistCfgDlgGtk::showErrorMessage(std::string const& msg, std::vector<std::string> const& values = std::vector<std::string>()){
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

void GrublistCfgDlgGtk::clear(){
	tvConfList.refTreeStore->clear();
}

void GrublistCfgDlgGtk::signal_info_click(){
	eventListener->aboutDialog_requested();
}

void GrublistCfgDlgGtk::signal_burg_switcher_response(int response_id){
	if (response_id == Gtk::RESPONSE_DELETE_EVENT)
		eventListener->burgSwitcher_cancelled();
	else
		eventListener->burgSwitcher_response(response_id == Gtk::RESPONSE_YES);
}

void GrublistCfgDlgGtk::signal_partition_chooser_question_response(int response_id){
	eventListener->partitionChooserQuestion_response(response_id == Gtk::RESPONSE_YES);
}

GrubConfListing::GrubConfListing(){
	refTreeStore = Gtk::TreeStore::create(treeModel);
	this->set_model(refTreeStore);

	this->append_column(this->mainColumn);
	this->mainColumn.pack_start(pixbufRenderer, false);
	this->mainColumn.add_attribute(pixbufRenderer.property_pixbuf(), treeModel.icon);
	this->mainColumn.pack_start(this->textRenderer, true);
	this->mainColumn.add_attribute(this->textRenderer.property_text(), treeModel.text);
	this->mainColumn.add_attribute(this->textRenderer.property_editable(), treeModel.is_editable);
	this->mainColumn.set_spacing(10);

	this->set_headers_visible(false);
	this->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	this->set_rubber_banding(true);
}

GrubConfListing::TreeModel::TreeModel(){
	this->add(name);
	this->add(text);
	this->add(relatedRule);
	this->add(is_other_entries_marker);
	this->add(is_editable);
	this->add(is_sensitive);
	this->add(icon);
}
