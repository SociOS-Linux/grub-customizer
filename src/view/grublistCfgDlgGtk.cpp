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
	: tbttAdd(Gtk::Stock::ADD), tbttRemove(Gtk::Stock::REMOVE), tbttUp(Gtk::Stock::GO_UP), tbttDown(Gtk::Stock::GO_DOWN),
	tbttLeft(Gtk::Stock::GO_BACK), tbttRight(Gtk::Stock::GO_FORWARD),
	tbttSave(Gtk::Stock::SAVE), tbttPreferences(Gtk::Stock::PREFERENCES),
	miFile(gettext("_File"), true), miExit(Gtk::Stock::QUIT), tbttReload(Gtk::Stock::REFRESH),
	miEdit(gettext("_Edit"), true), miView(gettext("_View"), true), miHelp(gettext("_Help"), true),
	miInstallGrub(gettext("_Install to MBR …"), true),
	miAdd(Gtk::Stock::ADD, Gtk::AccelKey('+', Gdk::CONTROL_MASK)), miRemove(Gtk::Stock::REMOVE, Gtk::AccelKey('-', Gdk::CONTROL_MASK)), miUp(Gtk::Stock::GO_UP, Gtk::AccelKey('u', Gdk::CONTROL_MASK)), miDown(Gtk::Stock::GO_DOWN, Gtk::AccelKey('d', Gdk::CONTROL_MASK)),
	miLeft(Gtk::Stock::GO_BACK, Gtk::AccelKey('l', Gdk::CONTROL_MASK)), miRight(Gtk::Stock::GO_FORWARD, Gtk::AccelKey('r', Gdk::CONTROL_MASK)),
	miPreferences(Gtk::Stock::PREFERENCES), miReload(Gtk::Stock::REFRESH, Gtk::AccelKey("F5")), miSave(Gtk::Stock::SAVE),
	miAbout(Gtk::Stock::ABOUT), miStartRootSelector(Gtk::Stock::OPEN),
	lock_state(~0), burgSwitcher(gettext("BURG found!"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO),
	pchooserQuestionDlg(gettext("No Bootloader found"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO)
{
	win.set_icon_name("grub-customizer");

	win.set_default_size(800,600);
	win.add(vbMainSplit);
	
	vbMainSplit.pack_start(menu, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(toolbar, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(scrEntryList);
	vbMainSplit.pack_start(statusbar, Gtk::PACK_SHRINK);

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

	toolbar.append(tbttAdd);
	tbttAdd.set_tooltip_text(gettext("Add a script to your configuration"));
	toolbar.append(tbttRemove);
	tbttRemove.set_tooltip_text(gettext("Remove a script from your configuration"));
	
	ti_sep2.add(vs_sep2);
	toolbar.append(ti_sep2);
	
	toolbar.append(tbttUp);
	tbttUp.set_tooltip_text(gettext("Move up the selected entry or script"));
	toolbar.append(tbttDown);
	tbttDown.set_tooltip_text(gettext("Move down the selected entry or script"));
	
	ti_sep5.add(vs_sep5);
	toolbar.append(ti_sep5);

	toolbar.append(tbttLeft);
	tbttLeft.set_tooltip_text(gettext("remove this entry from the current submenu"));

	toolbar.append(tbttRight);
	tbttRight.set_tooltip_text(gettext("add this entry to a new submenu"));

	ti_sep3.add(vs_sep3);
	toolbar.append(ti_sep3);
	
	toolbar.append(tbttReload);
	ti_sep4.add(vs_sep4);
	toolbar.append(ti_sep4);
	tbttReload.set_tooltip_text(gettext("reload configuration"));
	toolbar.append(tbttPreferences);
	tbttPreferences.set_is_important(true);
	tbttPreferences.set_tooltip_text(gettext("Edit grub preferences"));
	
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
	subEdit.attach(miPreferences, 0,1,6,7);
	
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
	
	tvConfList.refTreeStore->signal_row_changed().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_row_changed));
	tbttUp.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_click),-1));
	tbttDown.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_click),1));
	tvConfList.get_selection()->signal_changed().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_treeview_selection_changed));
	tbttSave.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::saveConfig));
	tbttAdd.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_add_click));
	tbttLeft.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_left_click));
	tbttRight.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_right_click));
	tbttReload.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_reload_click));
	tbttPreferences.signal_clicked().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_preference_click));
	
	miUp.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_click),-1));
	miDown.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_click),1));
	miLeft.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_left_click));
	miRight.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_move_right_click));
	miSave.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::saveConfig));
	miAdd.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_add_click));
	miReload.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_reload_click));
	miInstallGrub.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_show_grub_install_dialog_click));
	miStartRootSelector.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_show_root_selector));
	miPreferences.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_preference_click));

	miExit.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_quit_click));
	miAbout.signal_activate().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_info_click));
	
	burgSwitcher.signal_response().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_burg_switcher_response));
	pchooserQuestionDlg.signal_response().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_partition_chooser_question_response));

	win.signal_delete_event().connect(sigc::mem_fun(this, &GrublistCfgDlgGtk::signal_delete_event));

}

void GrublistCfgDlgGtk::setEventListener(EventListener_listCfgDlg& eventListener) {
	this->eventListener = &eventListener;
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
}

void GrublistCfgDlgGtk::progress_pulse(){
	progressBar.pulse();
	progressBar.show();
}

void GrublistCfgDlgGtk::hideProgressBar(){
	progressBar.hide();
}

void GrublistCfgDlgGtk::setStatusText(std::string const& new_status_text){
	statusbar.push(new_status_text);
}

void GrublistCfgDlgGtk::appendEntry(std::string const& name, bool is_active, void* entryPtr, bool is_placeholder, bool is_submenu, std::string const& scriptName, std::string const& defaultName, void* parentEntry){
	if (is_active) {
		Gtk::TreeIter entryRow;
		if (parentEntry) {
			entryRow = tvConfList.refTreeStore->append(this->getIterByRulePtr(parentEntry)->children());
		} else {
			entryRow = tvConfList.refTreeStore->append();
		}

		std::string outputName = name + "\n";
		if (is_submenu) {
			outputName += gettext("submenu");
		} else if (is_placeholder) {
			outputName += gettext("placeholder");
		} else {
			outputName += gettext("menuentry");
		}
		if (scriptName != "") {
			outputName += std::string(" / ") + gettext("script: ") + scriptName;
		}

		if (defaultName != "" && name != defaultName) {
			outputName += std::string(" / ") + gettext("default name: ") + defaultName;
		}

		(*entryRow)[tvConfList.treeModel.active] = is_active;
		(*entryRow)[tvConfList.treeModel.name] = outputName;
		(*entryRow)[tvConfList.treeModel.relatedRule] = (void*)entryPtr;
		(*entryRow)[tvConfList.treeModel.is_editable] = !is_placeholder && !is_submenu;
		(*entryRow)[tvConfList.treeModel.is_sensitive] = !is_placeholder;
		(*entryRow)[tvConfList.treeModel.font_weight] = is_submenu ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL;
		(*entryRow)[tvConfList.treeModel.fontStyle] = Pango::STYLE_NORMAL;

		tvConfList.expand_all();
	}
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
	tbttPreferences.set_sensitive((state & 8) == 0);
	miPreferences.set_sensitive((state & 8) == 0);

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

void GrublistCfgDlgGtk::signal_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter){
	if (this->lock_state == 0){
		eventListener->signal_entry_renamed((void*)(*iter)[tvConfList.treeModel.relatedRule]);
		eventListener->signal_entry_state_toggled((void*)(*iter)[tvConfList.treeModel.relatedRule]);
	}
}

void GrublistCfgDlgGtk::signal_show_root_selector(){
	eventListener->rootSelector_request();
}



std::string GrublistCfgDlgGtk::getRuleName(void* rule){
	Gtk::TreeModel::iterator iter = this->getIterByRulePtr(rule);
	return (Glib::ustring)(*iter)[tvConfList.treeModel.name];
}
void GrublistCfgDlgGtk::setRuleName(void* rule, std::string const& newName){
	Gtk::TreeModel::iterator iter = this->getIterByRulePtr(rule);
	this->setLockState(~0);
	(*iter)[tvConfList.treeModel.name] = newName;
	this->setLockState(0);
}

bool GrublistCfgDlgGtk::getRuleState(void* rule){
	Gtk::TreeModel::iterator iter = this->getIterByRulePtr(rule);
	return (*iter)[tvConfList.treeModel.active];
}
void GrublistCfgDlgGtk::setRuleState(void* rule, bool newState){
	Gtk::TreeModel::iterator iter = this->getIterByRulePtr(rule);
	this->setLockState(~0);
	(*iter)[tvConfList.treeModel.active] = newState;
	if ((*iter)[tvConfList.treeModel.is_sensitive]) {
		this->setEntrySensibility(iter->children(), newState);
	}
	this->setLockState(0);
}

void GrublistCfgDlgGtk::selectRule(void* rule, bool startEdit) {
	try {
		this->tvConfList.get_selection()->select(this->getIterByRulePtr(rule));
		if (startEdit) {
			this->tvConfList.set_cursor(this->tvConfList.refTreeStore->get_path(this->getIterByRulePtr(rule)), *this->tvConfList.get_column(1), true);
		}
	} catch (GrublistCfgDlg::Exception e) {
		if (e != RULE_ITER_NOT_FOUND)
			throw e;
	}
}

void GrublistCfgDlgGtk::setEntrySensibility(const Gtk::TreeNodeChildren& list, bool sensibility) {
	for (Gtk::TreeModel::iterator rule_iter = list.begin(); rule_iter != list.end(); rule_iter++) {
		(*rule_iter)[tvConfList.treeModel.is_sensitive] = sensibility;
		if (!sensibility || (*rule_iter)[tvConfList.treeModel.active] == sensibility)
			this->setEntrySensibility(rule_iter->children(), sensibility);
	}
}


void GrublistCfgDlgGtk::signal_move_click(int direction){
	if (this->lock_state == 0){
		if (direction == 1 || direction == -1){
			Gtk::TreeModel::iterator iter = tvConfList.get_selection()->get_selected();
			//if rule swap
			eventListener->ruleAdjustment_requested((void*)(*iter)[tvConfList.treeModel.relatedRule], direction);
		}
		else {
			this->log("the only supported directions for moving are 1 or -1", Logger::ERROR);
		}
	}
}

void GrublistCfgDlgGtk::update_remove_button(){
	if (tvConfList.get_selection()->count_selected_rows() == 1){
		if (tvConfList.get_selection()->get_selected()->parent() == false){ //wenn Script markiert
			tbttRemove.set_sensitive(true);
			miRemove.set_sensitive(true);
		}
		else {
			tbttRemove.set_sensitive(false);
			miRemove.set_sensitive(false);
		}
	}
	else {
		tbttRemove.set_sensitive(false);
		miRemove.set_sensitive(false);
	}
}

void GrublistCfgDlgGtk::setDefaultTitleStatusText(std::string const& str){
	this->setStatusText(gettext("Default title: ")+str);
}

void GrublistCfgDlgGtk::signal_treeview_selection_changed(){
	if (this->lock_state == 0){
		if (tvConfList.get_selection()->count_selected_rows() == 1){
			if (tvConfList.get_selection()->get_selected()->parent()){
				void* rptr = (*tvConfList.get_selection()->get_selected())[tvConfList.treeModel.relatedRule];
				this->eventListener->ruleSelected(rptr);
			}
		}

		this->updateButtonsState();
	}
}

void GrublistCfgDlgGtk::signal_add_click(){
	eventListener->scriptAddDlg_requested();
}

void GrublistCfgDlgGtk::signal_preference_click(){
	eventListener->settings_dialog_request();
}

void GrublistCfgDlgGtk::update_move_buttons(){
	int selectedRowsCount = tvConfList.get_selection()->count_selected_rows();
	bool is_toplevel = false;
	bool is_secondLevel = false;
	if (selectedRowsCount > 0) {
		is_toplevel = tvConfList.get_selection()->get_selected()->parent() ? false : true;
		if (!is_toplevel) {
			is_secondLevel = tvConfList.get_selection()->get_selected()->parent()->parent() ? false : true;
		}
	}
	bool isFirstProxy = false;
	bool isLastProxy = false;
	if (tvConfList.get_selection()->get_selected()) {
		if (is_toplevel && tvConfList.get_selection()->get_selected() == tvConfList.refTreeStore->children().begin()) {
			isFirstProxy = true;
		}
		Gtk::TreeModel::iterator iter = tvConfList.get_selection()->get_selected();
		iter++;
		if (is_toplevel && iter == tvConfList.refTreeStore->children().end()) {
			isLastProxy = true;
		}
	} else {
		isFirstProxy = true;
		isLastProxy = true;
	}

	tbttUp.set_sensitive(selectedRowsCount == 1 && !isFirstProxy);
	miUp.set_sensitive(selectedRowsCount == 1 && !isFirstProxy);
	tbttDown.set_sensitive(selectedRowsCount == 1 && !isLastProxy);
	miDown.set_sensitive(selectedRowsCount == 1 && !isLastProxy);
	tbttLeft.set_sensitive(selectedRowsCount == 1 && !is_toplevel && !is_secondLevel); //selected entry must be inside a submenu
	miLeft.set_sensitive(selectedRowsCount == 1 && !is_toplevel && !is_secondLevel); //selected entry must be inside a submenu
	tbttRight.set_sensitive(selectedRowsCount == 1 && !is_toplevel);
	miRight.set_sensitive(selectedRowsCount == 1 && !is_toplevel);
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
	Gtk::TreeModel::iterator iter = tvConfList.get_selection()->get_selected();
	eventListener->removeSubmenuRequest((void*)(*iter)[tvConfList.treeModel.relatedRule]);
}

void GrublistCfgDlgGtk::signal_move_right_click() {
	Gtk::TreeModel::iterator iter = tvConfList.get_selection()->get_selected();
	eventListener->createSubmenuRequest((void*)(*iter)[tvConfList.treeModel.relatedRule]);
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
	this->append_column_editable(gettext("name"), treeModel.name); //rows with is_editable==true will be made editable by cell renderer option
	
	{
		Gtk::TreeViewColumn* pColumn = this->get_column(0);
		Gtk::CellRendererText* pRenderer = static_cast<Gtk::CellRendererText*>(pColumn->get_first_cell_renderer());
		pColumn->add_attribute(pRenderer->property_editable(), treeModel.is_editable);
		pColumn->add_attribute(pRenderer->property_sensitive(), treeModel.is_sensitive);
	}

	{
		Gtk::TreeViewColumn* pColumn = this->get_column(0);
		Gtk::CellRendererText* pRenderer = static_cast<Gtk::CellRendererText*>(pColumn->get_first_cell_renderer());
		pColumn->add_attribute(pRenderer->property_weight(), treeModel.font_weight);
	}
	{
		Gtk::TreeViewColumn* pColumn = this->get_column(0);
		Gtk::CellRendererText* pRenderer = static_cast<Gtk::CellRendererText*>(pColumn->get_first_cell_renderer());
		pColumn->add_attribute(pRenderer->property_style(), treeModel.fontStyle);
	}
}

GrubConfListing::TreeModel::TreeModel(){
	this->add(active);
	this->add(name);
	this->add(relatedRule);
	this->add(is_other_entries_marker);
	this->add(is_editable);
	this->add(is_sensitive);
	this->add(font_weight);
	this->add(fontStyle);
}
