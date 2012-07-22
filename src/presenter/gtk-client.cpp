#include "gtk-client.h"

GtkClient::GtkClient(GrubEnv& env)
	: grublistCfg(NULL), listCfgDlg(NULL), settingsDlg(NULL), settings(NULL),
	  installer(NULL), installDlg(NULL), settingsOnDisk(NULL), scriptAddDlg(NULL),
	  partitionChooser(NULL), savedListCfg(NULL),
	 env(env), config_has_been_different_on_startup_but_unsaved(false),
	 modificationsUnsaved(false), quit_requested(false), activeThreadCount(0)
{
	disp_sync_load.connect(sigc::mem_fun(this, &GtkClient::syncListView_load));
	disp_sync_save.connect(sigc::mem_fun(this, &GtkClient::syncListView_save));
	disp_thread_died.connect(sigc::mem_fun(this, &GtkClient::thread_died_handler));
	disp_updateSettingsDlgResolutionList.connect(sigc::mem_fun(this, &GtkClient::updateSettingsDlgResolutionList_dispatched));
}


void GtkClient::setModelListCfg(GrublistCfg& grublistCfg){
	this->grublistCfg = &grublistCfg;
}
void GtkClient::setViewListCfg(GrublistCfgDlg& listCfgDlg){
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
void GtkClient::setScriptAddDlg(ScriptAddDlg& scriptAddDlg){
	this->scriptAddDlg = &scriptAddDlg;
}

void GtkClient::setSavedListCfg(GrublistCfg& savedListCfg){
	this->savedListCfg = &savedListCfg;
}

void GtkClient::setPartitionChooser(PartitionChooser& partitionChooser){
	this->partitionChooser = &partitionChooser;
}

void GtkClient::setFbResolutionsGetter(FbResolutionsGetter& fbResolutionsGetter){
	this->fbResolutionsGetter = &fbResolutionsGetter;
}

void GtkClient::showSettingsDlg(){
	this->settingsDlg->show();

	std::list<std::string> entryTitles = this->grublistCfg->proxies.generateEntryTitleList();
	this->settingsDlg->clearDefaultEntryChooser();
	for (std::list<std::string>::iterator iter = entryTitles.begin(); iter != entryTitles.end(); iter++)
		this->settingsDlg->addEntryToDefaultEntryChooser(*iter);

	this->settingsDlg->run();
	this->settingsDlg->hide();
	
	if (!this->modificationsUnsaved)
		this->modificationsUnsaved = settings->getIsModified();
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

void GtkClient::load(bool preserveConfig){
	this->activeThreadCount++;
	this->listCfgDlg->setLockState(5);
	
	if (!preserveConfig){
		this->grublistCfg->reset();
		//load the burg/grub settings file
		this->settings->load();
	}
	else {
		this->settingsOnDisk->load();
		this->settings->save();
	}
	this->grublistCfg->load(preserveConfig);
	if (!preserveConfig){
		if (this->savedListCfg->loadStaticCfg())
			this->config_has_been_different_on_startup_but_unsaved = !this->grublistCfg->compare(*this->savedListCfg);
		else
			this->config_has_been_different_on_startup_but_unsaved = false;
	}
	if (preserveConfig){
		this->settingsOnDisk->save();
	}
	this->activeThreadCount--;

	//loading the framebuffer resolutions in background…
	Glib::Thread::create(sigc::mem_fun(this->fbResolutionsGetter, &FbResolutionsGetter::load), false);
}

void GtkClient::save(){
	this->config_has_been_different_on_startup_but_unsaved = false;
	this->modificationsUnsaved = false; //deprecated

	this->listCfgDlg->setLockState(5);
	
	Glib::Thread::create(sigc::mem_fun(this, &GtkClient::save_thread), false);
}

void GtkClient::save_thread(){
	this->activeThreadCount++;
	this->settings->save();
	this->grublistCfg->save();
	this->activeThreadCount--;
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
				bool selectRoot = listCfgDlg->requestForRootSelection();
				if (!selectRoot)
					return false;
			}

			root = partitionChooser->run();

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
			listCfgDlg->setIsBurgMode(mode == GrubEnv::BURG_MODE);
			
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
		this->syncListView_load();
		listCfgDlg->setLockState(3);
	}
}

void GtkClient::syncEntryList(){
	this->disp_sync_load();
}

void GtkClient::updateSaveProgress(){
	this->disp_sync_save();
}

void GtkClient::showErrorThreadDied(){
	this->disp_thread_died();
}

void GtkClient::showInstallDialog(){
	installDlg->show();
}

void GtkClient::installGrub(std::string device){
	this->activeThreadCount++;
	installer->threadable_install(device);
	this->activeThreadCount--;
	if (this->activeThreadCount == 0 && this->quit_requested)
		this->listCfgDlg->close();
}

void GtkClient::showMessageGrubInstallCompleted(std::string const& msg){
	installDlg->showMessageGrubInstallCompleted(msg);
}

void GtkClient::showScriptAddDlg(){
	if (grublistCfg->repository.size() > 0){
		scriptAddDlg->clear();
		for (Repository::iterator iter = grublistCfg->repository.begin(); iter != grublistCfg->repository.end(); iter++){
			scriptAddDlg->addItem(iter->name);
		}
	}
	else
		Gtk::MessageDialog(gettext("No script found")).run();
	
	
	scriptAddDlg->show();
}

void GtkClient::addScriptFromScriptAddDlg(){
	Script* script = grublistCfg->repository.getNthScript(scriptAddDlg->getSelectedEntryIndex());
	grublistCfg->proxies.push_back(Proxy(*script));
	grublistCfg->renumerate();
	
	this->syncListView_load();
	
	this->modificationsUnsaved = true;
}

void GtkClient::updateScriptAddDlgPreview(){
	scriptAddDlg->clearPreview();
	Script* selectedScript = grublistCfg->repository.getNthScript(scriptAddDlg->getSelectedEntryIndex());
	if (selectedScript){
		for (Script::iterator iter = selectedScript->begin(); iter != selectedScript->end(); iter++)
			scriptAddDlg->addToPreview(iter->name);
	}
}

void GtkClient::removeProxy(Proxy* p){
	this->grublistCfg->proxies.deleteProxy(p);
	this->listCfgDlg->removeProxy(p);
	this->modificationsUnsaved = true;
}

void GtkClient::syncListView_load(){
	this->listCfgDlg->setLockState(5);
	double progress = this->grublistCfg->getProgress();
	if (progress != 1){
		this->listCfgDlg->setProgress(progress);
		this->listCfgDlg->setStatusText(gettext("loading configuration…"));
	}
	else {
		if (this->quit_requested)
			this->listCfgDlg->close();
		this->listCfgDlg->hideProgressBar();
		this->listCfgDlg->setStatusText("");
	}
	
	//if grubConfig is locked, it will be cancelled as early as possible
	if (this->grublistCfg->lock_if_free()){
		this->listCfgDlg->clear();
	
		for (std::list<Proxy>::iterator iter = this->grublistCfg->proxies.begin(); iter != this->grublistCfg->proxies.end(); iter++){
			Glib::ustring name = iter->getScriptName() + (this->grublistCfg && iter->dataSource && (progress != 1 && iter->dataSource->fileName != iter->fileName || progress == 1 && grublistCfg->proxies.proxyRequired(*iter->dataSource)) ? gettext(" (custom)") : "");
			this->listCfgDlg->appendScript(name, iter->isExecutable(), &(*iter));
			for (std::list<Rule>::iterator ruleIter = iter->rules.begin(); ruleIter != iter->rules.end(); ruleIter++){
				bool is_other_entries_ph = ruleIter->type == Rule::OTHER_ENTRIES_PLACEHOLDER;
				if (ruleIter->dataSource || is_other_entries_ph){
					Glib::ustring name = is_other_entries_ph ? gettext("(new Entries)") : ruleIter->outputName;
					this->listCfgDlg->appendEntry(name, ruleIter->isVisible, &(*ruleIter), !is_other_entries_ph);
				}
			}
		}
		this->grublistCfg->unlock();
	}

	if (progress == 1){
		this->listCfgDlg->setLockState(0);
	}
}

void GtkClient::syncListView_save(){
	this->listCfgDlg->progress_pulse();
	if (this->grublistCfg->getProgress() == 1){
		if (this->grublistCfg->error_proxy_not_found){
			this->listCfgDlg->showProxyNotFoundMessage();
			this->grublistCfg->error_proxy_not_found = false;
		}
		if (this->quit_requested)
			listCfgDlg->close();
		
		this->listCfgDlg->setLockState(0);
		
		this->listCfgDlg->hideProgressBar();
		this->listCfgDlg->setStatusText(gettext("Configuration has been saved"));
	}
	else {
		this->listCfgDlg->setStatusText(gettext("updating configuration"));
	}
}

//TODO: rename
void GtkClient::thread_died_handler(){
	this->listCfgDlg->showErrorMessage(this->grublistCfg->getMessage());
	this->listCfgDlg->close(); //exit
}

bool GtkClient::quit(){
	int dlgResponse = this->listCfgDlg->showExitConfirmDialog(this->config_has_been_different_on_startup_but_unsaved*2 + this->modificationsUnsaved);
	if (dlgResponse == 1){
		this->save(); //starts a thread that delays the application exiting
	}
	
	if (dlgResponse != 0){
		if (this->activeThreadCount != 0){
			this->quit_requested = true;
			this->grublistCfg->cancelThreads();
			return true;
		}
		else
			return false; //close the window
	}
	return true;
}

//MOVE TO PRESENTER
void GtkClient::syncProxyState(void* proxy){
	((Proxy*)proxy)->set_isExecutable(this->listCfgDlg->getProxyState(proxy));
	this->modificationsUnsaved = true;
}

//MOVE TO PRESENTER
void GtkClient::syncRuleState(Rule* entry){
	entry->isVisible = this->listCfgDlg->getRuleState(entry);
	this->modificationsUnsaved = true;
	this->updateScriptEntry(this->grublistCfg->proxies.getProxyByRule(entry));
}

//MOVE TO PRESENTER
void GtkClient::syncRuleName(Rule* entry){
	Glib::ustring oldName = entry->outputName;
	Glib::ustring newName = this->listCfgDlg->getRuleName(entry);
	if (newName == ""){
		Gtk::MessageDialog(gettext("Name the Entry")).run();
		this->listCfgDlg->setRuleName(entry, oldName);
		this->updateScriptEntry(this->grublistCfg->proxies.getProxyByRule(entry));
	}
	else {
		this->renameEntry(entry, newName);
	}
	this->modificationsUnsaved = true;
}

//MOVE TO PRESENTER
void GtkClient::updateScriptEntry(Proxy* proxy){
	//adding (custom) if this script is modified
	if (proxy->dataSource){ //checking the Datasource before Accessing it
		Glib::ustring name = proxy->dataSource->name;
		this->listCfgDlg->setProxyName(proxy, name, false);
		if (this->grublistCfg->proxies.proxyRequired(*proxy->dataSource)){
			this->listCfgDlg->setProxyName(proxy, name, true);
		}
	}
}

//MOVE TO PRESENTER
void GtkClient::swapRules(Rule* a, Rule* b){
	//swap the contents behind the pointers
	grublistCfg->swapRules(a, b);
	this->listCfgDlg->swapRules(a,b);
	this->updateScriptEntry(this->grublistCfg->proxies.getProxyByRule(a));
	this->modificationsUnsaved = true;
}

//MOVE TO PRESENTER
void GtkClient::swapProxies(Proxy* a, Proxy* b){
	grublistCfg->swapProxies(a,b);
	this->listCfgDlg->swapProxies(a,b);
	this->modificationsUnsaved = true;
}


//MOVE TO EVENT_LISTENER
void GtkClient::showRuleInfo(Rule* rule){
	if (rule && rule->dataSource)
		this->listCfgDlg->setDefaultTitleStatusText(rule->getEntryName());
	else
		this->listCfgDlg->setStatusText("");
	this->listCfgDlg->updateButtonsState();
}

//MOVE TO EVENT_LISTENER
void GtkClient::showProxyInfo(Proxy* proxy){
	this->listCfgDlg->setStatusText("");
	this->listCfgDlg->updateButtonsState();
}

void GtkClient::updateSettingsDlgResolutionList(){
	this->disp_updateSettingsDlgResolutionList();
}

void GtkClient::updateSettingsDlgResolutionList_dispatched(){
	const std::list<std::string>& data = this->fbResolutionsGetter->getData();
	this->settingsDlg->clearResolutionChooser();
	for (std::list<std::string>::const_iterator iter = data.begin(); iter != data.end(); iter++)
		this->settingsDlg->addResolution(*iter);
}
