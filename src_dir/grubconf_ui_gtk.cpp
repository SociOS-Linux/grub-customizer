#include "grubconf_ui_gtk.h"

ImageMenuItemOwnKey::ImageMenuItemOwnKey(const Gtk::StockID& id, const Gtk::AccelKey& accel_key) : Gtk::ImageMenuItem(id){
	set_accel_key(accel_key);
}

GrubConfUIGtk::GrubConfUIGtk(GrubConfig& grubConfig)
	: grubConfig(&grubConfig), appName("Grub Customizer"), appVersion("1.6"),
	tbttAdd(Gtk::Stock::ADD), tbttRemove(Gtk::Stock::REMOVE), tbttUp(Gtk::Stock::GO_UP), tbttDown(Gtk::Stock::GO_DOWN),
	tbttSave(Gtk::Stock::SAVE), tbttPreferences(Gtk::Stock::PREFERENCES),
	miFile(gettext("_File"), true), miExit(Gtk::Stock::QUIT), tbttReload(Gtk::Stock::REFRESH),
	miEdit(gettext("_Edit"), true), miView(gettext("_View"), true), miHelp(gettext("_Help"), true),
	miInstallGrub(gettext("_Install to MBR …"), true),
	miAdd(Gtk::Stock::ADD, Gtk::AccelKey('+', Gdk::CONTROL_MASK)), miRemove(Gtk::Stock::REMOVE, Gtk::AccelKey('-', Gdk::CONTROL_MASK)), miUp(Gtk::Stock::GO_UP, Gtk::AccelKey('u', Gdk::CONTROL_MASK)), miDown(Gtk::Stock::GO_DOWN, Gtk::AccelKey('d', Gdk::CONTROL_MASK)),
	miPreferences(Gtk::Stock::PREFERENCES), miReload(Gtk::Stock::REFRESH, Gtk::AccelKey("F5")), miSave(Gtk::Stock::SAVE),
	miAbout(Gtk::Stock::ABOUT), miStartRootSelector(Gtk::Stock::OPEN),
	completelyLoaded(false),
	lvScriptPreview(1), lblScriptSelection(gettext("Script to insert:"), Gtk::ALIGN_LEFT), lblScriptPreview(gettext("Preview:"),Gtk::ALIGN_LEFT),
	thread_active(false), quit_requested(false), modificationsUnsaved(false),
	lblGrubInstallDescription(gettext("Install the bootloader to MBR and put some\nfiles to the bootloaders data directory\n(if they don't already exist)."), Gtk::ALIGN_LEFT),
	lblGrubInstallDevice(gettext("_Device: "), Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, true)
{
	disp_update_load.connect(sigc::mem_fun(this, &GrubConfUIGtk::update));
	disp_update_save.connect(sigc::mem_fun(this, &GrubConfUIGtk::update_save));
	disp_thread_died.connect(sigc::mem_fun(this, &GrubConfUIGtk::thread_died_handler));
	disp_grub_install_ready.connect(sigc::mem_fun(this, &GrubConfUIGtk::func_disp_grub_install_ready));

	win.set_icon_name("grub-customizer");

	authors.push_back("Daniel Richter");
	if (this->grubConfig->env.burgMode)
		win.set_title("Grub Customizer (BURG Mode)");
	else
		win.set_title("Grub Customizer");
	win.set_default_size(800,600);
	win.add(vbMainSplit);
	
	vbMainSplit.pack_start(menu, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(toolbar, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(scrEntryList);
	vbMainSplit.pack_start(statusbar, Gtk::PACK_SHRINK);

	scrEntryList.add(tvConfList);
	statusbar.add(progressBar);
	
	tvConfList.set_sensitive(false);
	scrEntryList.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrEntryList.set_shadow_type(Gtk::SHADOW_IN);
	
	progressBar.set_pulse_step(0.1);
	
	dlgAbout.set_name(appName);
	dlgAbout.set_version(appVersion);
	dlgAbout.set_authors(authors);
	
	dlgAbout.set_icon_name("grub-customizer");
	dlgAbout.set_logo_icon_name("grub-customizer");
	dlgAbout.set_comments(gettext("Grub Customizer is a graphical interface to configure the grub2/burg settings"));
	
	//toolbar
	toolbar.append(tbttSave);
	tbttSave.set_tooltip_text(Glib::ustring(gettext("Save configuration and generate a new "))+(this->grubConfig->env.burgMode?"burg.cfg":"grub.cfg"));
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
	tbttReload.set_tooltip_text(gettext("reload configuration (same effect as an application restart)"));
	//toolbar.append(tbttPreferences);
	tbttPreferences.set_tooltip_text(gettext("Edit grub preferences"));
	
	disableButtons();
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
	//subEdit.attach(miPreferences, 0,1,4,5);
	
	subView.attach(miReload, 0,1,0,1);
	
	subHelp.attach(miAbout, 0,1,0,1);
	
	miStartRootSelector.set_label(gettext("Select _partition …"));
	miStartRootSelector.set_use_underline(true);
	
	miExit.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_quit_click));
	miAbout.signal_activate().connect(sigc::mem_fun(&dlgAbout, &Gtk::AboutDialog::show_all));

	//Script/Proxy-add-Window
	scriptAddDlg.set_title(gettext("Add script"));
	scriptAddDlg.set_icon_name("grub-customizer");
	scriptAddDlg.set_default_size(400, 300);
	Gtk::VBox* vbScriptAddDlg = scriptAddDlg.get_vbox();
	vbScriptAddDlg->set_spacing(10);
	vbScriptAddDlg->pack_start(hbScriptSelection, Gtk::PACK_SHRINK);
	hbScriptSelection.set_border_width(10);
	hbScriptSelection.pack_start(lblScriptSelection);
	hbScriptSelection.pack_start(cbScriptSelection);
	vbScriptAddDlg->pack_start(vbScriptPreview);
	vbScriptPreview.pack_start(lblScriptPreview, Gtk::PACK_SHRINK);
	vbScriptPreview.pack_start(scrScriptPreview);
	scrScriptPreview.add(lvScriptPreview);
	vbScriptPreview.set_border_width(10);
	scrScriptPreview.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrScriptPreview.set_shadow_type(Gtk::SHADOW_IN);
	lvScriptPreview.set_column_title(0, gettext("Entry"));
	lvScriptPreview.set_headers_visible(false);
	
	scriptAddDlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	scriptAddDlg.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	
	//grub-install-dialog
	Gtk::VBox* vbGrubInstallDialog = grubInstallDialog.get_vbox();
	grubInstallDialog.set_icon_name("grub-customizer");
	vbGrubInstallDialog->pack_start(lblGrubInstallDescription, Gtk::PACK_SHRINK);
	vbGrubInstallDialog->pack_start(hbGrubInstallDevice);
	vbGrubInstallDialog->pack_start(lblInstallInfo);
	hbGrubInstallDevice.pack_start(lblGrubInstallDevice, Gtk::PACK_SHRINK);
	hbGrubInstallDevice.pack_start(txtGrubInstallDevice);
	txtGrubInstallDevice.set_text("/dev/sda");
	grubInstallDialog.set_title(gettext("Install to MBR"));
	vbGrubInstallDialog->set_spacing(5);
	lblGrubInstallDevice.set_mnemonic_widget(txtGrubInstallDevice);
	grubInstallDialog.set_border_width(5);
	grubInstallDialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	grubInstallDialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	grubInstallDialog.set_default_response(Gtk::RESPONSE_OK);
	txtGrubInstallDevice.set_activates_default(true);
	
	//signals
	
	tvConfList.refTreeStore->signal_row_changed().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_row_changed));
	tbttUp.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &GrubConfUIGtk::signal_move_click),-1));
	tbttDown.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &GrubConfUIGtk::signal_move_click),1));
	tvConfList.get_selection()->signal_changed().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_treeview_selection_changed));
	tbttSave.signal_clicked().connect(sigc::mem_fun(this, &GrubConfUIGtk::saveConfig));
	tbttAdd.signal_clicked().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_add_click));
	tbttRemove.signal_clicked().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_remove_click));
	tbttReload.signal_clicked().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_reload_click));
	
	miUp.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &GrubConfUIGtk::signal_move_click),-1));
	miDown.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &GrubConfUIGtk::signal_move_click),1));
	miSave.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::saveConfig));
	miAdd.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_add_click));
	miRemove.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_remove_click));
	miReload.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_reload_click));
	miInstallGrub.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_show_grub_install_dialog_click));
	miStartRootSelector.signal_activate().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_show_root_selector));
	
	cbScriptSelection.signal_changed().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_script_selection_changed));
	
	scriptAddDlg.signal_response().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_scriptAddDlg_response));
	grubInstallDialog.signal_response().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_grub_install_dialog_response));
	
	win.signal_delete_event().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_delete_event));

	dlgAbout.signal_response().connect(sigc::mem_fun(this, &GrubConfUIGtk::signal_about_dlg_response));

	thread_active = true;
}

void GrubConfUIGtk::event_load_progress_changed(){
	disp_update_load();
}
void GrubConfUIGtk::event_save_progress_changed(){
	disp_update_save();
}

void GrubConfUIGtk::event_thread_died(){
	disp_thread_died();
}

void GrubConfUIGtk::event_grub_install_ready(){
	disp_grub_install_ready();
}

bool GrubConfUIGtk::bootloader_not_found_requestForRootSelection(){
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

std::string GrubConfUIGtk::show_root_selector(){
	LiveCDSetupDialog setupDialog(GrubConfEnvironment::isLiveCD());
	setupDialog.readPartitionInfo();
	setupDialog.show();
	Gtk::Main::run(setupDialog);
	if (setupDialog.isCancelled())
		return "";
	else
		return setupDialog.getRootMountpoint();
}

void GrubConfUIGtk::func_disp_grub_install_ready(){
	std::string output = grubConfig->install_result;
	if (output == ""){
		Gtk::MessageDialog msg(gettext("The bootloader has been installed successfully"));
		msg.run();
		grubInstallDialog.hide();
	}
	else {
		Gtk::MessageDialog msg(gettext("Error while installing the bootloader"), false, Gtk::MESSAGE_ERROR);
		msg.set_secondary_text(output);
		msg.run();
	}
	grubInstallDialog.set_response_sensitive(Gtk::RESPONSE_OK, true);
	grubInstallDialog.set_response_sensitive(Gtk::RESPONSE_CANCEL, true);
	txtGrubInstallDevice.set_sensitive(true);
	lblInstallInfo.set_text("");
}

void GrubConfUIGtk::run(){
	win.show_all();
	Glib::Thread::create(sigc::mem_fun(grubConfig, &GrubConfig::load), false);
	Gtk::Main::run(win);
}

void GrubConfUIGtk::update(){
	if (!completelyLoaded){
		double progress = grubConfig->getProgress();
		if (progress != 1){
			progressBar.set_fraction(progress);
			progressBar.show();
			statusbar.push(gettext("loading configuration…"));
		}
		else {
			thread_active = false;
			if (quit_requested)
				win.hide();
			tvConfList.set_sensitive(true);
			progressBar.hide();
			statusbar.push("");
			tbttSave.set_sensitive(true);
			miSave.set_sensitive(true);
			tbttAdd.set_sensitive(true);
			miAdd.set_sensitive(true);
			tbttReload.set_sensitive(true);
			miReload.set_sensitive(true);
			
			miStartRootSelector.set_sensitive(true);
		}
		
		//if grubConfig is locked, it will be cancelled as early as possible
		if (!grubConfig->read_lock){
			grubConfig->write_lock = true;
			tvConfList.refTreeStore->clear();
		
			for (std::list<Proxy>::iterator iter = grubConfig->proxies.begin(); !grubConfig->read_lock && iter != grubConfig->proxies.end(); iter++){
				Gtk::TreeIter row = tvConfList.refTreeStore->append();
				(*row)[tvConfList.treeModel.active] = iter->isExecutable();
			
				//if the config is loading, only compare the files… else call the more complex proxyRequired method
				(*row)[tvConfList.treeModel.name] = iter->getScriptName() + (grubConfig && iter->dataSource && (progress != 1 && iter->dataSource->fileName != iter->fileName || progress == 1 && grubConfig->proxies.proxyRequired(*iter->dataSource)) ? gettext(" (custom)") : "");
				
				(*row)[tvConfList.treeModel.relatedRule] = NULL;
				(*row)[tvConfList.treeModel.relatedProxy] = &(*iter);
				(*row)[tvConfList.treeModel.is_other_entries_marker] = false;
				(*row)[tvConfList.treeModel.is_editable] = false;
				int i = 0;
				for (std::list<Rule>::iterator ruleIter = iter->rules.begin(); !grubConfig->read_lock && ruleIter != iter->rules.end(); ruleIter++){
					bool is_other_entries_ph = ruleIter->type == Rule::OTHER_ENTRIES_PLACEHOLDER;
					if (ruleIter->dataSource || is_other_entries_ph){
						Gtk::TreeIter entryRow = tvConfList.refTreeStore->append(row->children());
						(*entryRow)[tvConfList.treeModel.active] = ruleIter->isVisible;
						(*entryRow)[tvConfList.treeModel.name] = is_other_entries_ph ? gettext("(new Entries)") : ruleIter->outputName;
						(*entryRow)[tvConfList.treeModel.relatedRule] = &(*ruleIter);
						(*entryRow)[tvConfList.treeModel.relatedProxy] = &(*iter);
						(*entryRow)[tvConfList.treeModel.is_editable] = !is_other_entries_ph;
					}
				}
			}
			grubConfig->write_lock = false;
			tvConfList.expand_all();
		}
	
		if (progress == 1){
			completelyLoaded = true;
		}
	}
}

void GrubConfUIGtk::update_save(){
	progressBar.pulse();
	if (grubConfig->getProgress() == 1){
		if (grubConfig->error_proxy_not_found){
			Gtk::MessageDialog msg(gettext("Proxy binary not found!"), false, Gtk::MESSAGE_WARNING);
			msg.set_secondary_text(gettext("You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly."));
			msg.run();
			grubConfig->error_proxy_not_found = false;
		}
		thread_active = false;
		if (quit_requested)
			win.hide();
		tbttSave.set_sensitive(true);
		miSave.set_sensitive(true);
		tbttReload.set_sensitive(true);
		miReload.set_sensitive(true);
		miStartRootSelector.set_sensitive(true);
		tbttAdd.set_sensitive(true);
		miAdd.set_sensitive(true);
		tvConfList.set_sensitive(true);
		progressBar.hide();
		statusbar.push(gettext("Configuration has been saved"));
	}
	else {
		statusbar.push(gettext("updating configuration"));
		progressBar.show();
	}
}

void GrubConfUIGtk::thread_died_handler(){
	Gtk::MessageDialog(grubConfig->getMessage(), false, Gtk::MESSAGE_ERROR).run();
	win.hide(); //exit
}

void GrubConfUIGtk::disableButtons(){
	tbttSave.set_sensitive(false);
	miSave.set_sensitive(false);

	tbttUp.set_sensitive(false);
	miUp.set_sensitive(false);
	tbttDown.set_sensitive(false);
	miDown.set_sensitive(false);

	tbttAdd.set_sensitive(false);
	miAdd.set_sensitive(false);
	tbttRemove.set_sensitive(false);
	miRemove.set_sensitive(false);
	
	tbttReload.set_sensitive(false);
	miReload.set_sensitive(false);
	miStartRootSelector.set_sensitive(false);
	tbttPreferences.set_sensitive(false);
	miPreferences.set_sensitive(false);
}

void GrubConfUIGtk::saveConfig(){
	tvConfList.set_sensitive(false);
	disableButtons();
	thread_active = true;
	Glib::Thread::create(sigc::mem_fun(grubConfig, &GrubConfig::save), false);
	modificationsUnsaved = false;
}


void GrubConfUIGtk::signal_reload_click(){
	disableButtons();
	tvConfList.set_sensitive(false);
	grubConfig->reset();
	completelyLoaded = false;
	Glib::Thread::create(sigc::mem_fun(grubConfig, &GrubConfig::load), false);
}

void GrubConfUIGtk::signal_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter){
	if (completelyLoaded){
		if (iter->parent()){ //if it's a rule row (no proxy)
			Glib::ustring oldName = ((Rule*)(*iter)[tvConfList.treeModel.relatedRule])->outputName;
			Glib::ustring newName = (*iter)[tvConfList.treeModel.name];
			if (oldName != newName){
				if (newName == ""){
					Gtk::MessageDialog(gettext("Name the Entry")).run();
					(*iter)[tvConfList.treeModel.name] = oldName; //reset name
				}
				else {
					((Rule*)(*iter)[tvConfList.treeModel.relatedRule])->outputName = (Glib::ustring)(*iter)[tvConfList.treeModel.name];
				}
			}
			((Rule*)(*iter)[tvConfList.treeModel.relatedRule])->isVisible = (*iter)[tvConfList.treeModel.active];
			
			Proxy* relatedProxy = ((Proxy*)(*iter)[tvConfList.treeModel.relatedProxy]);
			
			if (((Proxy*)(*iter->parent())[tvConfList.treeModel.relatedProxy])->dataSource){ //checking the Datasource before Accessing it
				completelyLoaded = false;
				(*iter->parent())[tvConfList.treeModel.name] = ((Proxy*)(*iter->parent())[tvConfList.treeModel.relatedProxy])->dataSource->name;
				if (grubConfig->proxies.proxyRequired((*((Proxy*)(*iter->parent())[tvConfList.treeModel.relatedProxy])->dataSource))){
					(*iter->parent())[tvConfList.treeModel.name] = (*iter->parent())[tvConfList.treeModel.name] + " (angepasst)";
				}
				completelyLoaded = true;
			}
		}
		else {
			((Proxy*)(*iter)[tvConfList.treeModel.relatedProxy])->set_isExecutable((*iter)[tvConfList.treeModel.active]);
		}
		
		modificationsUnsaved = true;
	}
}

void GrubConfUIGtk::signal_show_root_selector(){
	std::string old_partition = grubConfig->env.getRootDevice();
	bool done = grubConfig->prepare(true);
	std::string new_partition = grubConfig->env.getRootDevice();
	
	if (done || old_partition != new_partition && new_partition != ""){
		signal_reload_click();
		miInstallGrub.set_sensitive(true);
	}
	else if (new_partition == ""){ //this happens, when a previously selected partition has been umounted
		grubConfig->reset();
		completelyLoaded = false;
		this->update();
		completelyLoaded = true;
		
		disableButtons();
		tvConfList.set_sensitive(false);
		miStartRootSelector.set_sensitive(true);
		miInstallGrub.set_sensitive(false);
	}
}

void GrubConfUIGtk::signal_move_click(int direction){
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
			//swap the contents behind the pointers
			Rule swap_helper = *a;
			*a = *b;
			*b = swap_helper;
			
			//swap the assigned pointers
			(*iter)[tvConfList.treeModel.relatedRule] = b;
			(*iter2)[tvConfList.treeModel.relatedRule] = a;
		}
		else { //if script swap
			grubConfig->increaseProxyPos((Proxy*)((*(direction == 1 ? iter : iter2))[tvConfList.treeModel.relatedProxy]));
		}
		tvConfList.refTreeStore->iter_swap(iter, iter2);
		//std::cout << "direction: " << direction << std::endl;
		
		update_move_buttons();
	}
	else
		std::cerr << "the only supported directions for moving are 1 or -1" << std::endl;
	
	modificationsUnsaved = true;
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

void GrubConfUIGtk::signal_treeview_selection_changed(){
	if (completelyLoaded){
		update_move_buttons();
		update_remove_button();
		
		bool info_set = false;
		if (tvConfList.get_selection()->count_selected_rows() == 1){
			if (tvConfList.get_selection()->get_selected()->parent()){
				Rule* rptr = (*tvConfList.get_selection()->get_selected())[tvConfList.treeModel.relatedRule];
				if (rptr && rptr->dataSource){
					statusbar.push(gettext("Default title: ")+rptr->getScriptName());
					info_set = true;
				}
			}
		}
		if (!info_set)
			statusbar.push("");
	}
}

void GrubConfUIGtk::signal_add_click(){
	if (grubConfig->repository.size() > 0){
		cbScriptSelection.clear_items();
		for (Repository::iterator iter = grubConfig->repository.begin(); iter != grubConfig->repository.end(); iter++){
			cbScriptSelection.append_text(iter->name);
		}
		cbScriptSelection.set_active(0);
		scriptAddDlg.show_all();
	}
	else
		Gtk::MessageDialog(gettext("No script found")).run();
}

void GrubConfUIGtk::signal_script_selection_changed(){
	lvScriptPreview.clear_items();
	Script* selectedScript = grubConfig->repository.getNthScript(cbScriptSelection.get_active_row_number());
	if (selectedScript){
		for (Script::iterator iter = selectedScript->begin(); iter != selectedScript->end(); iter++)
			lvScriptPreview.append_text(iter->name);
	}
	lvScriptPreview.columns_autosize();
}

void GrubConfUIGtk::signal_scriptAddDlg_response(int response_id){
	if (response_id == Gtk::RESPONSE_OK){
		Script* script = grubConfig->repository.getNthScript(cbScriptSelection.get_active_row_number());
		grubConfig->proxies.push_back(Proxy(*script));
		grubConfig->renumerate();
		
		completelyLoaded = false;
		update();
		completelyLoaded = true;
		
		modificationsUnsaved = true;
	}
	scriptAddDlg.hide();
}

void GrubConfUIGtk::signal_remove_click(){
	Proxy* proxyPointer = (Proxy*)(*(tvConfList.get_selection()->get_selected()))[tvConfList.treeModel.relatedProxy];
	this->grubConfig->proxies.deleteProxy(proxyPointer);
	
	completelyLoaded = false;
	update();
	completelyLoaded = true;
	
	update_remove_button();
	modificationsUnsaved = true;
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

void GrubConfUIGtk::signal_quit_click(){
	if (this->signal_delete_event(NULL) == false)
		win.hide();
}

bool GrubConfUIGtk::signal_delete_event(GdkEventAny* event){ //return value: keep window open
	int dlgResponse = Gtk::RESPONSE_NO;
	if (grubConfig->config_has_been_different_on_startup_but_unsaved || modificationsUnsaved){
		Gtk::MessageDialog msgDlg("", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_NONE);
		if (grubConfig->config_has_been_different_on_startup_but_unsaved){
			msgDlg.set_message(gettext("The saved configuration is not up to date!"));
			msgDlg.set_secondary_text(gettext("The generated configuration didn't equal to the saved configuration on startup. So what you see now may not be what you see when you restart your pc. To fix this, click update!"));
			
			Gtk::Button* btnQuit = msgDlg.add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_NO);
			btnQuit->set_label(gettext("_Quit without update"));
			msgDlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			Gtk::Button* btnSave = msgDlg.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_YES);
			btnSave->set_label(gettext("_Update & Quit"));
		}
		if (modificationsUnsaved && !grubConfig->config_has_been_different_on_startup_but_unsaved){
			msgDlg.property_message_type() = Gtk::MESSAGE_QUESTION;
			msgDlg.set_message(gettext("Do you want to save your modifications?"));

			Gtk::Button* btnQuit = msgDlg.add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_NO);
			btnQuit->set_label(gettext("_Quit without saving"));
			msgDlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			Gtk::Button* btnSave = msgDlg.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_YES);
			btnSave->set_label(gettext("_Save & Quit"));
		}
		if (grubConfig->config_has_been_different_on_startup_but_unsaved && modificationsUnsaved){
			msgDlg.set_secondary_text(msgDlg.property_secondary_text()+"\n\nAND: your modifications are still unsaved, update will save them too!");
		}

		msgDlg.set_default_response(Gtk::RESPONSE_YES);

		dlgResponse = msgDlg.run();
	}
	
	if (dlgResponse == Gtk::RESPONSE_YES){
		this->saveConfig(); //starts a thread that delays the application exiting
	}
	
	if (dlgResponse != Gtk::RESPONSE_CANCEL){
		if (thread_active){
			quit_requested = true;
			grubConfig->cancelThreads();
			return true;
		}
		else
			return false; //close the window
	}
	return true;
}

void GrubConfUIGtk::signal_about_dlg_response(int response_id){
	if (Gtk::RESPONSE_CLOSE)
		dlgAbout.hide();
}

void GrubConfUIGtk::signal_show_grub_install_dialog_click(){
	grubInstallDialog.show_all();
}

void GrubConfUIGtk::signal_grub_install_dialog_response(int response_id){
	if (response_id == Gtk::RESPONSE_OK){
		if (txtGrubInstallDevice.get_text().length()){
			grubInstallDialog.set_response_sensitive(Gtk::RESPONSE_OK, false);
			grubInstallDialog.set_response_sensitive(Gtk::RESPONSE_CANCEL, false);
			txtGrubInstallDevice.set_sensitive(false);
			lblInstallInfo.set_text(gettext("installing the bootloader…"));
			
			Glib::Thread::create(sigc::bind<std::string>(sigc::mem_fun(grubConfig, &GrubConfig::threadable_install), txtGrubInstallDevice.get_text()), false);
		}
		else
			Gtk::MessageDialog(gettext("Please type a device string!")).run();
	}
	else
		grubInstallDialog.hide();
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
