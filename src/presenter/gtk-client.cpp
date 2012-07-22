#include "gtk-client.h"

GtkClient::GtkClient(GrubEnv& env)
	: grublistCfg(NULL), listCfgDlg(NULL), settingsDlg(NULL), settings(NULL),
	  installer(NULL), installDlg(NULL),
	 env(env)
{
}


void GtkClient::setModelListCfg(GrublistCfg& grublistCfg){
	this->grublistCfg = &grublistCfg;
}
void GtkClient::setViewListCfg(GrubConfUIGtk& listCfgDlg){
	this->listCfgDlg = &listCfgDlg;
}

void GtkClient::setViewSettingsDialog(GrubSettingsDlgGtk& settingsDlg){
	this->settingsDlg = &settingsDlg;
}

void GtkClient::setModelSettingsManager(SettingsManagerDataStore& settings){
	this->settings = &settings;
}

void GtkClient::setSettingsBuffer(SettingsManagerDataStore& settings){
	this->settingsOnDisk = &settings;
}

void GtkClient::setInstaller(GrubInstaller& installer){
	this->installer = &installer;
}
void GtkClient::setInstallDlg(GrubInstallDlg& installDlg){
	this->installDlg = &installDlg;
}

void GtkClient::showSettingsDlg(){
	this->settingsDlg->show(this->grublistCfg->proxies.generateEntryTitleList());
	this->settingsDlg->run();
	this->settingsDlg->hide();
	
	if (!this->listCfgDlg->modificationsUnsaved)
		this->listCfgDlg->modificationsUnsaved = settings->getIsModified();
	if (this->settings->reloadRequired()){
		Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &GtkClient::load), true), false);
	}
}

void GtkClient::run(){
	bool do_continue = this->prepare();
	if (do_continue){
		Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &GtkClient::load), false), false);
		this->listCfgDlg->run();
	}
	this->grublistCfg->umountSwitchedRootPartition(); //cleanup… only if another partition has been mounted
}

void GtkClient::load(bool keepConfig){
	this->listCfgDlg->setLockState(5);
	
	if (!keepConfig){
		this->grublistCfg->reset();
		//load the burg/grub settings file
		this->settings->load();
	}
	else {
		this->settingsOnDisk->load();
		this->settings->save();
	}
	this->grublistCfg->load(keepConfig);

	if (keepConfig){
		this->settingsOnDisk->save();
	}
}

void GtkClient::save(){
	this->listCfgDlg->thread_active = true; //deprecated
	this->listCfgDlg->modificationsUnsaved = false; //deprecated

	this->listCfgDlg->setLockState(5);
	this->settings->save();
	this->grublistCfg->save();
}

void GtkClient::renameEntry(Rule* rule, std::string const& newName){
	if (this->settings->getValue("GRUB_DEFAULT") == rule->outputName)
		this->settings->setValue("GRUB_DEFAULT", newName);
	this->grublistCfg->renameRule(rule, newName);
}

void GtkClient::reset(){
	this->grublistCfg->reset();
	this->settings->clear();
}

bool GtkClient::prepare(bool forceRootSelection){
	bool exit = true;
	bool firstRun = true;
	std::string root = env.cfg_dir_prefix;
	do {
		exit = true;
		
		bool burg_found = env.init(GrubEnv::BURG_MODE, root);
		bool grub_found = env.init(GrubEnv::GRUB_MODE, root);
		bool isLiveCD = GrubEnv::isLiveCD();
		if (forceRootSelection || isLiveCD && firstRun || !burg_found && !grub_found){
			if (!forceRootSelection && !burg_found && !grub_found && (!isLiveCD || !firstRun)){
				bool selectRoot = listCfgDlg->bootloader_not_found_requestForRootSelection();
				if (!selectRoot)
					return false;
			}
			root = listCfgDlg->show_root_selector();
			if (root == "")
				return false;
			else {
				forceRootSelection = false;
				exit = false;
			}
		}
		else if (grub_found || burg_found){
			GrubEnv::Mode mode = GrubEnv::GRUB_MODE;
			if (grub_found && burg_found){
				if (listCfgDlg->requestForBurgMode())
					mode = GrubEnv::BURG_MODE;
			}
			else if (burg_found)
				mode = GrubEnv::BURG_MODE;
			
			env.init(mode, root);
			listCfgDlg->event_mode_changed();
			
			if (this->grublistCfg->cfgDirIsClean() == false)
				this->grublistCfg->cleanupCfgDir();
		}
		
		firstRun = false;
	}
	while (!exit);
	return true;
}

void GtkClient::startRootSelector(){
	std::string old_partition = this->env.getRootDevice();
	bool done = this->prepare(true);
	std::string new_partition = this->env.getRootDevice();
	
	if (done || old_partition != new_partition && new_partition != ""){
		Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &GtkClient::load), false), false);
		listCfgDlg->setLockState(5);
	}
	else if (new_partition == ""){ //this happens, when a previously selected partition has been umounted
		this->reset();
		listCfgDlg->update();

		listCfgDlg->setLockState(3);
	}
}

void GtkClient::syncEntryList(){
	listCfgDlg->event_load_progress_changed();
}

void GtkClient::updateSaveProgress(){
	listCfgDlg->event_save_progress_changed();
}

void GtkClient::showErrorThreadDied(){
	listCfgDlg->event_thread_died();
}

void GtkClient::showInstallDialog(){
	installDlg->show();
}

void GtkClient::installGrub(std::string const& device){
	Glib::Thread::create(sigc::bind<std::string>(sigc::mem_fun(installer, &GrubInstaller::threadable_install), device), false);
}

void GtkClient::showMessageGrubInstallCompleted(std::string const& msg){
	installDlg->showMessageGrubInstallCompleted(msg);
}



