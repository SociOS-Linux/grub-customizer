#include "grubconf_ui_gtk.h"

ImageMenuItemOwnKey::ImageMenuItemOwnKey(const Gtk::StockID& id, const Gtk::AccelKey& accel_key) : Gtk::ImageMenuItem(id){
	set_accel_key(accel_key);
}

GrubConfUIGtk::GrubConfUIGtk(GrublistCfg& grubConfig)
	: grubConfig(&grubConfig), appName("Grub Customizer"), appVersion("2.0.6"),
	tbttAdd(Gtk::Stock::ADD), tbttRemove(Gtk::Stock::REMOVE), tbttUp(Gtk::Stock::GO_UP), tbttDown(Gtk::Stock::GO_DOWN),
	tbttSave(Gtk::Stock::SAVE), tbttPreferences(Gtk::Stock::PREFERENCES),
	miFile(gettext("_File"), true), miExit(Gtk::Stock::QUIT), tbttReload(Gtk::Stock::REFRESH),
	miEdit(gettext("_Edit"), true), miView(gettext("_View"), true), miHelp(gettext("_Help"), true),
	miInstallGrub(gettext("_Install to MBR …"), true),
	miAdd(Gtk::Stock::ADD, Gtk::AccelKey('+', Gdk::CONTROL_MASK)), miRemove(Gtk::Stock::REMOVE, Gtk::AccelKey('-', Gdk::CONTROL_MASK)), miUp(Gtk::Stock::GO_UP, Gtk::AccelKey('u', Gdk::CONTROL_MASK)), miDown(Gtk::Stock::GO_DOWN, Gtk::AccelKey('d', Gdk::CONTROL_MASK)),
	miPreferences(Gtk::Stock::PREFERENCES), miReload(Gtk::Stock::REFRESH, Gtk::AccelKey("F5")), miSave(Gtk::Stock::SAVE),
	miAbout(Gtk::Stock::ABOUT), miStartRootSelector(Gtk::Stock::OPEN),
	thread_active(false), quit_requested(false), modificationsUnsaved(false), lock_state(~0)
{
	win.set_icon_name("grub-customizer");

	authors.push_back("Daniel Richter https://launchpad.net/~danielrichter2007");
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
	
	dlgAbout.set_name(appName);
	dlgAbout.set_version(appVersion);
	dlgAbout.set_authors(authors);
	
	dlgAbout.set_icon_name("grub-customizer");
	dlgAbout.set_logo_icon_name("grub-customizer");
	dlgAbout.set_comments(gettext("Grub Customizer is a graphical interface to configure the grub2/burg settings"));
	
	dlgAbout.set_translator_credits(
"Adam Czabara https://launchpad.net/~adam-czabara\n\
Alexey Ivanov https://launchpad.net/~alexey.ivanes\n\
Bernardo Miguel Savone https://launchpad.net/~bersil\n\
Careone https://launchpad.net/~zzbusagain\n\
Daniel Richter https://launchpad.net/~danielrichter2007\n\
Emre AYTAÇ https://launchpad.net/~eaytac\n\
Erkin Batu Altunbaş https://launchpad.net/~erkin\n\
Eugênio F https://launchpad.net/~eugf\n\
Fedik https://launchpad.net/~fedikw\n\
GamePad64 https://launchpad.net/~gamepad64\n\
GoJoMo https://launchpad.net/~tolbkni\n\
José Humberto Melo https://launchpad.net/~josehumberto-melo\n\
Manuel Xosé Lemos https://launchpad.net/~mxlemos\n\
Maxime Gentils https://launchpad.net/~maxime.gentils\n\
Michael Kotsarinis https://launchpad.net/~mk73628\n\
Miguel Anxo Bouzada https://launchpad.net/~mbouzada\n\
nafterburner https://launchpad.net/~nafterburner\n\
patel https://launchpad.net/~patel\n\
shishimaru https://launchpad.net/~salvi-uchiha\n\
Svetoslav Stefanov https://launchpad.net/~svetlisashkov\n\
TheMengzor https://launchpad.net/~the-mengzor\n\
zeugma https://launchpad.net/~sunder67\
");
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
	subEdit.attach(miPreferences, 0,1,4,5);
	
	subView.attach(miReload, 0,1,0,1);
	
	subHelp.attach(miAbout, 0,1,0,1);
	
	miStartRootSelector.set_label(gettext("Select _partition …"));
	miStartRootSelector.set_use_underline(true);
	
	miExit.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_quit_click));
	miAbout.signal_activate().connect(sigc::mem_fun(&dlgAbout, &Gtk::AboutDialog::show_all));


	//signals
	
	tvConfList.refTreeStore->signal_row_changed().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_row_changed));
	tbttUp.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &GrubConfUIGtk::signal_move_click),-1));
	tbttDown.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &GrubConfUIGtk::signal_move_click),1));
	tvConfList.get_selection()->signal_changed().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_treeview_selection_changed));
	tbttSave.signal_clicked().connect(sigc::mem_fun(this, &GrubConfUIGtk::saveConfig));
	tbttAdd.signal_clicked().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_add_click));
	tbttRemove.signal_clicked().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_remove_click));
	tbttReload.signal_clicked().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_reload_click));
	tbttPreferences.signal_clicked().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_preference_click));
	
	miUp.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &GrubConfUIGtk::signal_move_click),-1));
	miDown.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &GrubConfUIGtk::signal_move_click),1));
	miSave.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::saveConfig));
	miAdd.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_add_click));
	miRemove.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_remove_click));
	miReload.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_reload_click));
	miInstallGrub.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_show_grub_install_dialog_click));
	miStartRootSelector.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_show_root_selector));
	miPreferences.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_preference_click));
	
	win.signal_delete_event().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_delete_event));

	dlgAbout.signal_response().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_about_dlg_response));

	thread_active = true;
}

void GrubConfUIGtk::setEventListener(EventListenerView_iface& eventListener) {
	this->eventListener = &eventListener;
}

void GrubConfUIGtk::setIsBurgMode(bool isBurgMode){
	if (isBurgMode)
		win.set_title("Grub Customizer (" + Glib::ustring(gettext("BURG Mode")) + ")");
	else
		win.set_title("Grub Customizer");

	tbttSave.set_tooltip_text(Glib::ustring(gettext("Save configuration and generate a new "))+(isBurgMode?"burg.cfg":"grub.cfg"));
}


bool GrubConfUIGtk::requestForRootSelection(){
	Gtk::MessageDialog dlg(gettext("No Bootloader found"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
	dlg.set_secondary_text(gettext("Do you want to select another root partition?"));
	dlg.set_default_response(Gtk::RESPONSE_YES);
	int result = dlg.run();
	return result == Gtk::RESPONSE_YES;
}

bool GrubConfUIGtk::requestForBurgMode(){
	Gtk::MessageDialog dlg(gettext("BURG found!"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
	dlg.set_secondary_text(gettext("Do you want to configure BURG instead of grub2?"));
	dlg.set_default_response(Gtk::RESPONSE_YES);
	int result = dlg.run();
	return result == Gtk::RESPONSE_YES;
}

//TODO: Partition chooser should be a member!
std::string GrubConfUIGtk::show_root_selector(){
	PartitionChooser setupDialog(GrubEnv::isLiveCD());
	setupDialog.readPartitionInfo();
	setupDialog.show();
	Gtk::Main::run(setupDialog);
	if (setupDialog.isCancelled())
		return "";
	else
		return setupDialog.getRootMountpoint();
}


void GrubConfUIGtk::run(){
	win.show_all();
	Gtk::Main::run(win);
}

void GrubConfUIGtk::setProgress(double progress){
	progressBar.set_fraction(progress);
	progressBar.show();
}

void GrubConfUIGtk::progress_pulse(){
	progressBar.pulse();
	progressBar.show();
}

void GrubConfUIGtk::hideProgressBar(){
	progressBar.hide();
}

void GrubConfUIGtk::setStatusText(Glib::ustring const& new_status_text){
	statusbar.push(new_status_text);
}

void GrubConfUIGtk::appendScript(Glib::ustring const& name, bool is_active, void* proxyPtr){
	Gtk::TreeIter row = tvConfList.refTreeStore->append();
	(*row)[tvConfList.treeModel.active] = is_active;

	//if the config is loading, only compare the files… else call the more complex proxyRequired method
	(*row)[tvConfList.treeModel.name] = name;
	
	(*row)[tvConfList.treeModel.relatedRule] = NULL;
	(*row)[tvConfList.treeModel.relatedProxy] = (Proxy*)proxyPtr;
	(*row)[tvConfList.treeModel.is_other_entries_marker] = false;
	(*row)[tvConfList.treeModel.is_editable] = false;
}

void GrubConfUIGtk::appendEntry(Glib::ustring const& name, bool is_active, void* entryPtr, bool editable){
	Gtk::TreeIter lastScriptIter = *tvConfList.refTreeStore->children().rbegin();
	
	Gtk::TreeIter entryRow = tvConfList.refTreeStore->append(lastScriptIter->children());
	(*entryRow)[tvConfList.treeModel.active] = is_active;
	(*entryRow)[tvConfList.treeModel.name] = name;
	(*entryRow)[tvConfList.treeModel.relatedRule] = (Rule*)entryPtr;
	(*entryRow)[tvConfList.treeModel.relatedProxy] = (Proxy*)(*lastScriptIter)[tvConfList.treeModel.relatedProxy];
	(*entryRow)[tvConfList.treeModel.is_editable] = editable;
	
	tvConfList.expand_all();
}



void GrubConfUIGtk::showProxyNotFoundMessage(){
	Gtk::MessageDialog msg(gettext("Proxy binary not found!"), false, Gtk::MESSAGE_WARNING);
	msg.set_secondary_text(gettext("You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly."));
	msg.run();
}

void GrubConfUIGtk::saveConfig(){
	eventListener->save_request();
}


void GrubConfUIGtk::setLockState(int state){
	//state(bin):
	//	0: lock nothing
	//	1: lock normal items/buttons
	//	2: lock grub-install
	//	4: lock partition chooser
	tbttSave.set_sensitive((state & 1) == 0);
	miSave.set_sensitive((state & 1) == 0);

	tbttUp.set_sensitive((state & 1) == 0);
	miUp.set_sensitive((state & 1) == 0);
	tbttDown.set_sensitive((state & 1) == 0);
	miDown.set_sensitive((state & 1) == 0);

	tbttAdd.set_sensitive((state & 1) == 0);
	miAdd.set_sensitive((state & 1) == 0);
	tbttRemove.set_sensitive((state & 1) == 0);
	miRemove.set_sensitive((state & 1) == 0);
	
	tbttReload.set_sensitive((state & 1) == 0);
	miReload.set_sensitive((state & 1) == 0);
	miStartRootSelector.set_sensitive((state & 4) == 0);
	tbttPreferences.set_sensitive((state & 1) == 0);
	miPreferences.set_sensitive((state & 1) == 0);

	tvConfList.set_sensitive((state & 1) == 0);
	
	
	miInstallGrub.set_sensitive((state & 2) == 0);
	
	if ((state & 1) == 0) {
		this->updateButtonsState();
	}
	
	this->lock_state = state;
}


void GrubConfUIGtk::updateButtonsState(){
	update_remove_button();
	update_move_buttons();
}

void GrubConfUIGtk::signal_reload_click(){
	eventListener->reload_request();
}

//MAKE PRIVATE
Gtk::TreeModel::iterator GrubConfUIGtk::getIterByScriptPtr(void* scriptPtr){
	Gtk::TreeModel::iterator iter = tvConfList.refTreeStore->children().begin();
	while (iter != tvConfList.refTreeStore->children().end()){
		if (!iter->parent() && (*iter)[tvConfList.treeModel.relatedProxy] == scriptPtr)
			break;
		iter++;
	}
	return iter;
}

//MAKE PRIVATE
Gtk::TreeModel::iterator GrubConfUIGtk::getIterByEntryPtr(void* entryPtr){
	Gtk::TreeModel::iterator iter = tvConfList.refTreeStore->children().begin();
	while (iter != tvConfList.refTreeStore->children().end()){
		for (Gtk::TreeModel::iterator iter2 = iter->children().begin(); iter2 != iter->children().end(); iter2++){
			if (iter2->parent() && (*iter2)[tvConfList.treeModel.relatedRule] == entryPtr)
				return iter2;
		}
	}
	return iter;
}

//MOVE TO EVENT_LISTENER
void GrubConfUIGtk::signal_script_state_toggled(void* script){
	Gtk::TreeModel::iterator iter = this->getIterByScriptPtr(script);
	((Proxy*)(*iter)[tvConfList.treeModel.relatedProxy])->set_isExecutable((*iter)[tvConfList.treeModel.active]);
	modificationsUnsaved = true;
}
//MOVE TO EVENT_LISTENER
void GrubConfUIGtk::signal_entry_state_toggled(void* entry){
	Gtk::TreeModel::iterator iter = this->getIterByEntryPtr(entry);
	((Rule*)(*iter)[tvConfList.treeModel.relatedRule])->isVisible = (*iter)[tvConfList.treeModel.active];
	updateScriptEntry(entry);
	modificationsUnsaved = true;
}

//MOVE TO EVENT_LISTENER
void GrubConfUIGtk::signal_entry_renamed(void* entry){
	Gtk::TreeModel::iterator iter = this->getIterByEntryPtr(entry);
	Glib::ustring oldName = ((Rule*)entry)->outputName;
	Glib::ustring newName = (*iter)[tvConfList.treeModel.name];
	if (newName == ""){
		Gtk::MessageDialog(gettext("Name the Entry")).run();
		(*iter)[tvConfList.treeModel.name] = oldName; //reset name
	}
	else {
		eventListener->entry_rename_request((*iter)[tvConfList.treeModel.relatedRule], (Glib::ustring)(*iter)[tvConfList.treeModel.name]);
	}
	updateScriptEntry(entry);
	modificationsUnsaved = true;
}

void GrubConfUIGtk::updateScriptEntry(void* entry){
	Gtk::TreeModel::iterator iter = getIterByEntryPtr(entry);
	Proxy* relatedProxy = ((Proxy*)(*iter)[tvConfList.treeModel.relatedProxy]);
	
	//adding (custom) if this script is modified
	if (((Proxy*)(*iter->parent())[tvConfList.treeModel.relatedProxy])->dataSource){ //checking the Datasource before Accessing it
		this->setLockState(~0);
		(*iter->parent())[tvConfList.treeModel.name] = ((Proxy*)(*iter->parent())[tvConfList.treeModel.relatedProxy])->dataSource->name;
		if (grubConfig->proxies.proxyRequired((*((Proxy*)(*iter->parent())[tvConfList.treeModel.relatedProxy])->dataSource))){
			(*iter->parent())[tvConfList.treeModel.name] = (*iter->parent())[tvConfList.treeModel.name] + gettext(" (custom)");
		}
		this->setLockState(0);
	}
}

void GrubConfUIGtk::signal_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter){
	if (this->lock_state == 0){
		if (iter->parent()){ //if it's a rule row (no proxy)
			Glib::ustring oldName = ((Rule*)(*iter)[tvConfList.treeModel.relatedRule])->outputName;
			Glib::ustring newName = (*iter)[tvConfList.treeModel.name];
			if (oldName != newName){
				signal_entry_renamed((Rule*)(*iter)[tvConfList.treeModel.relatedRule]);
			}
			signal_entry_state_toggled((Rule*)(*iter)[tvConfList.treeModel.relatedRule]);
		}
		else {
			signal_script_state_toggled((Proxy*)(*iter)[tvConfList.treeModel.relatedProxy]);
		}
	}
}

void GrubConfUIGtk::signal_show_root_selector(){
	eventListener->rootSelector_request();
}


//MOVE TO PRESENTER
void GrubConfUIGtk::ruleSwap_requested(Rule* a, Rule* b){
	//swap the contents behind the pointers
	grubConfig->swapRules(a, b);
	this->swapRules(a,b);
}

//MOVE TO PRESENTER
void GrubConfUIGtk::proxySwap_requested(Proxy* a, Proxy* b){
	grubConfig->swapProxies(a,b);
	this->swapProxies(a,b);
}

void GrubConfUIGtk::swapProxies(Proxy* a, Proxy* b){
	tvConfList.refTreeStore->iter_swap(getIterByScriptPtr(a), getIterByScriptPtr(b));
	
	update_move_buttons();
	modificationsUnsaved = true;
}

void GrubConfUIGtk::swapRules(Rule* a, Rule* b){
	Gtk::TreeModel::iterator iter1 = getIterByEntryPtr(a);
	Gtk::TreeModel::iterator iter2 = getIterByEntryPtr(b);
	
	tvConfList.refTreeStore->iter_swap(iter1, iter2);
	//swap the assigned pointers
	(*iter1)[tvConfList.treeModel.relatedRule] = b;
	(*iter2)[tvConfList.treeModel.relatedRule] = a;
	
	update_move_buttons();
	updateScriptEntry(a);
	updateScriptEntry(b);
	modificationsUnsaved = true;
}

void GrubConfUIGtk::signal_move_click(int direction){
	if (this->lock_state == 0){
		if (direction == 1 || direction == -1){
			Gtk::TreeModel::iterator iter = tvConfList.get_selection()->get_selected();
			Gtk::TreeModel::iterator iter2 = iter;
			if (direction == 1){
				iter2++;
			}
			else if (direction == -1){
				iter2--;
			}
		
			//if rule swap
			if ((Rule*)(*iter)[tvConfList.treeModel.relatedRule] != NULL){
				Rule* a = ((Rule*)(*iter)[tvConfList.treeModel.relatedRule]);
				Rule* b = ((Rule*)(*iter2)[tvConfList.treeModel.relatedRule]);
			
				ruleSwap_requested(a, b);

			}
			else { //if script swap
				Proxy* a = (*iter)[tvConfList.treeModel.relatedProxy];
				Proxy* b = (*iter2)[tvConfList.treeModel.relatedProxy];
			
				proxySwap_requested(a, b);
			}
		}
		else
			std::cerr << "the only supported directions for moving are 1 or -1" << std::endl;
	}
}

void GrubConfUIGtk::update_remove_button(){
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

void GrubConfUIGtk::setDefaultTitleStatusText(Glib::ustring const& str){
	this->setStatusText(gettext("Default title: ")+str);
}

//MOVE TO EVENT_LISTENER
void GrubConfUIGtk::ruleSelected(Rule* rule){
	if (rule && rule->dataSource)
		this->setDefaultTitleStatusText(rule->getEntryName());
	else
		this->setStatusText("");
	this->updateButtonsState();
}

//MOVE TO EVENT_LISTENER
void GrubConfUIGtk::proxySelected(Proxy* proxy){
	this->setStatusText("");
	this->updateButtonsState();
}

void GrubConfUIGtk::signal_treeview_selection_changed(){
	if (this->lock_state == 0){
		bool rule_ev_executed = false;
		if (tvConfList.get_selection()->count_selected_rows() == 1){
			if (tvConfList.get_selection()->get_selected()->parent()){
				Rule* rptr = (*tvConfList.get_selection()->get_selected())[tvConfList.treeModel.relatedRule];
				this->ruleSelected(rptr);
				rule_ev_executed = true;
			}
		}
		if (!rule_ev_executed)
			this->proxySelected((*tvConfList.get_selection()->get_selected())[tvConfList.treeModel.relatedProxy]);
	}
}

void GrubConfUIGtk::signal_add_click(){
	eventListener->scriptAddDlg_requested();
}

void GrubConfUIGtk::removeProxy(Proxy* p){
	//Parameter is not really required
	this->setLockState(~0);
	Gtk::TreeModel::iterator iter = getIterByScriptPtr(p);
	tvConfList.refTreeStore->erase(iter);
	this->setLockState(0);
	
	update_remove_button();
	modificationsUnsaved = true;
}

void GrubConfUIGtk::signal_remove_click(){
	Proxy* proxyPointer = (Proxy*)(*(tvConfList.get_selection()->get_selected()))[tvConfList.treeModel.relatedProxy];
	eventListener->removeProxy_requested(proxyPointer);
}

void GrubConfUIGtk::signal_preference_click(){
	eventListener->settings_dialog_request();
}

void GrubConfUIGtk::update_move_buttons(){
	int selectedRowsCount = tvConfList.get_selection()->count_selected_rows();

	tbttUp.set_sensitive(selectedRowsCount == 1);
	miUp.set_sensitive(selectedRowsCount == 1);
	tbttDown.set_sensitive(selectedRowsCount == 1);
	miDown.set_sensitive(selectedRowsCount == 1);
	
	if (selectedRowsCount == 1){
		Gtk::TreeModel::iterator selectedRowIter = tvConfList.get_selection()->get_selected();
	
		if (selectedRowIter->parent()){
			if (selectedRowIter->parent()->children().begin() == selectedRowIter){
				tbttUp.set_sensitive(false);
				miUp.set_sensitive(false);
			}
			if (--selectedRowIter->parent()->children().end() == selectedRowIter){
				tbttDown.set_sensitive(false);
				miDown.set_sensitive(false);
			}
		}
		else {
			if (tvConfList.refTreeStore->children().begin() == selectedRowIter){
				tbttUp.set_sensitive(false);
				miUp.set_sensitive(false);
			}
			if (--tvConfList.refTreeStore->children().end() == selectedRowIter){
				tbttDown.set_sensitive(false);
				miDown.set_sensitive(false);
			}
		}
	}
}


void GrubConfUIGtk::close(){
	win.hide();
}

/**
 * @param type int: which type of dialog to show (1: changes unsaved, 2: conf not up to date, 3: 1 + 2)
 * @return int: type of the answer: 0: cancel, 1: yes, 2: no
 */
int GrubConfUIGtk::showExitConfirmDialog(int type){
	int dlgResponse = Gtk::RESPONSE_NO;
	if (type & 3 != 0){
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

bool GrubConfUIGtk::signal_delete_event(GdkEventAny* event){ //return value: keep window open
	return eventListener->exitRequest();
}

void GrubConfUIGtk::signal_quit_click(){
	if (eventListener->exitRequest() == false)
		this->close();
}

void GrubConfUIGtk::signal_about_dlg_response(int response_id){
	if (Gtk::RESPONSE_CLOSE)
		dlgAbout.hide();
}

void GrubConfUIGtk::signal_show_grub_install_dialog_click(){
	eventListener->installDialogRequest();
}

void GrubConfUIGtk::showErrorMessage(Glib::ustring const& msg){
	Gtk::MessageDialog(msg, false, Gtk::MESSAGE_ERROR).run();
}

void GrubConfUIGtk::clear(){
	tvConfList.refTreeStore->clear();
}

GrubConfListing::GrubConfListing(){
	refTreeStore = Gtk::TreeStore::create(treeModel);
	this->set_model(refTreeStore);
	this->append_column_editable(gettext("is active"), treeModel.active);
	this->append_column_editable(gettext("name"), treeModel.name); //rows with is_editable==true will be made editable by cell renderer option
	
	Gtk::TreeViewColumn* pColumn = this->get_column(1);
	Gtk::CellRendererText* pRenderer = static_cast<Gtk::CellRendererText*>(pColumn->get_first_cell_renderer());
	pColumn->add_attribute(pRenderer->property_editable(), treeModel.is_editable);
}

GrubConfListing::TreeModel::TreeModel(){
	this->add(active);
	this->add(name);
	this->add(relatedRule);
	this->add(relatedProxy);
	this->add(is_other_entries_marker);
	this->add(is_editable);
}
