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

#include "grubCustomizer.h"

GrubCustomizer::GrubCustomizer(GrubEnv& env)
	: grublistCfg(NULL), listCfgDlg(NULL), settingsDlg(NULL), settings(NULL),
	  installer(NULL), installDlg(NULL), settingsOnDisk(NULL), scriptAddDlg(NULL),
	  partitionChooser(NULL), savedListCfg(NULL),
	  fbResolutionsGetter(NULL), deviceDataList(NULL),
	  mountTable(NULL), aboutDialog(NULL),
	 env(env), config_has_been_different_on_startup_but_unsaved(false),
	 modificationsUnsaved(false), quit_requested(false), activeThreadCount(0),
	 is_loading(false)
{
	disp_sync_load.connect(sigc::mem_fun(this, &GrubCustomizer::syncListView_load));
	disp_sync_save.connect(sigc::mem_fun(this, &GrubCustomizer::syncListView_save));
	disp_thread_died.connect(sigc::mem_fun(this, &GrubCustomizer::die));
	disp_settings_loaded.connect(sigc::mem_fun(this, &GrubCustomizer::activateSettingsBtn));
	disp_updateSettingsDlgResolutionList.connect(sigc::mem_fun(this, &GrubCustomizer::updateSettingsDlgResolutionList_dispatched));
}


void GrubCustomizer::setListCfg(GrublistCfg& grublistCfg){
	this->grublistCfg = &grublistCfg;
}
void GrubCustomizer::setListCfgDlg(GrublistCfgDlg& listCfgDlg){
	this->listCfgDlg = &listCfgDlg;
}

void GrubCustomizer::setSettingsDialog(SettingsDlg& settingsDlg){
	this->settingsDlg = &settingsDlg;
}

void GrubCustomizer::setSettingsManager(SettingsManagerDataStore& settings){
	this->settings = &settings;
}

void GrubCustomizer::setSettingsBuffer(SettingsManagerDataStore& settings){
	this->settingsOnDisk = &settings;
}

void GrubCustomizer::setInstaller(GrubInstaller& installer){
	this->installer = &installer;
}
void GrubCustomizer::setInstallDlg(GrubInstallDlg& installDlg){
	this->installDlg = &installDlg;
}
void GrubCustomizer::setScriptAddDlg(ScriptAddDlg& scriptAddDlg){
	this->scriptAddDlg = &scriptAddDlg;
}

void GrubCustomizer::setSavedListCfg(GrublistCfg& savedListCfg){
	this->savedListCfg = &savedListCfg;
}

void GrubCustomizer::setPartitionChooser(PartitionChooser& partitionChooser){
	this->partitionChooser = &partitionChooser;
}

void GrubCustomizer::setFbResolutionsGetter(FbResolutionsGetter& fbResolutionsGetter){
	this->fbResolutionsGetter = &fbResolutionsGetter;
}

void GrubCustomizer::setDeviceDataList(DeviceDataList& deviceDataList){
	this->deviceDataList = &deviceDataList;
}

void GrubCustomizer::setMountTable(MountTable& mountTable){
	this->mountTable = &mountTable;
}

void GrubCustomizer::setAboutDialog(AboutDialog& aboutDialog){
	this->aboutDialog = &aboutDialog;
}

void GrubCustomizer::updateSettingsDlg(){
	std::list<std::string> entryTitles = this->grublistCfg->proxies.generateEntryTitleList();
	this->settingsDlg->clearDefaultEntryChooser();
	for (std::list<std::string>::iterator iter = entryTitles.begin(); iter != entryTitles.end(); iter++)
		this->settingsDlg->addEntryToDefaultEntryChooser(*iter);


	this->syncSettings();
}

void GrubCustomizer::init(){
	if   ( !grublistCfg
		or !listCfgDlg
		or !settingsDlg
		or !settings
		or !settingsOnDisk
		or !installer
		or !installDlg
		or !scriptAddDlg
		or !partitionChooser
		or !savedListCfg
		or !fbResolutionsGetter
		or !deviceDataList
		or !mountTable
		or !aboutDialog
	) {
		throw INCOMPLETE;
	}
	savedListCfg->verbose = false;

	FILE* blkidProc = popen("blkid", "r");
	if (blkidProc){
		deviceDataList->clear();
		deviceDataList->loadData(blkidProc);
		pclose(blkidProc);
	}

	mountTable->loadData("");
	mountTable->loadData(PARTCHOOSER_MOUNTPOINT);


	//loading the framebuffer resolutions in background…
	Glib::Thread::create(sigc::mem_fun(this->fbResolutionsGetter, &FbResolutionsGetter::load), false);

	//dir_prefix may be set by partition chooser (if not, the root partition is used)

	//aufs is the virtual root fileSystem used by live cds
	if (mountTable->getEntryByMountpoint("").isLiveCdFs() && env.cfg_dir_prefix == ""){
		partitionChooser->setIsStartedManually(false);
		this->initRootSelector();
		partitionChooser->show();
	}
	else {
		std::list<GrubEnv::Mode> modes = this->env.getAvailableModes();
		if (modes.size() == 2)
			this->listCfgDlg->showBurgSwitcher();
		else if (modes.size() == 1)
			this->init(modes.front());
		else if (modes.size() == 0)
			this->listCfgDlg->showPartitionChooserQuestion();
	}
}

void GrubCustomizer::init(GrubEnv::Mode mode){
	this->env.init(mode, env.cfg_dir_prefix);
	this->listCfgDlg->setLockState(1|4|8);
	this->listCfgDlg->setIsBurgMode(mode == GrubEnv::BURG_MODE);
	this->listCfgDlg->show();
	this->listCfgDlg->hideBurgSwitcher();

	if (this->grublistCfg->cfgDirIsClean() == false)
		this->grublistCfg->cleanupCfgDir();

	Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &GrubCustomizer::load), false), false);
}

void GrubCustomizer::hidePartitionChooserQuestion(){
	this->listCfgDlg->hidePartitionChooserQuestion();
}

void GrubCustomizer::showPartitionChooser(){
	partitionChooser->setIsStartedManually(true);
	this->initRootSelector();
	this->partitionChooser->show();
}

void GrubCustomizer::handleCancelResponse(){
	if (!listCfgDlg->isVisible())
		Gtk::Main::quit();
}

void GrubCustomizer::showSettingsDlg(){
	this->settingsDlg->show(env.burgMode);
}

void GrubCustomizer::reload(){
	this->syncSettings();
	this->listCfgDlg->setLockState(1|4|8);
	Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &GrubCustomizer::load), true), false);
}

//threaded!
void GrubCustomizer::load(bool preserveConfig){
	if (!is_loading){ //allow only one load thread at the same time!
		is_loading = true;
		this->activeThreadCount++;

		if (!preserveConfig){
			this->grublistCfg->reset();
			this->savedListCfg->reset();
			//load the burg/grub settings file
			this->settings->load();
			disp_settings_loaded();
		}
		else {
			this->settingsOnDisk->load();
			this->settings->save();
		}

		try {
			this->grublistCfg->load(preserveConfig);
		}
		catch (GrublistCfg::Exception e){
			this->thrownException = e;
			this->disp_thread_died();
			return; //cancel
		}
	
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
		this->is_loading = false;
	}
}

void GrubCustomizer::save(){
	this->config_has_been_different_on_startup_but_unsaved = false;
	this->modificationsUnsaved = false; //deprecated

	this->listCfgDlg->setLockState(1|4|8);

	this->activeThreadCount++; //not in save_thead() to be faster set
	Glib::Thread::create(sigc::mem_fun(this, &GrubCustomizer::save_thread), false);
}

void GrubCustomizer::save_thread(){
	this->settings->save();
	this->grublistCfg->save();
	this->activeThreadCount--;
}

void GrubCustomizer::renameEntry(Rule* rule, std::string const& newName){
	if (this->settings->getValue("GRUB_DEFAULT") == rule->outputName)
		this->settings->setValue("GRUB_DEFAULT", newName);
	this->grublistCfg->renameRule(rule, newName);
}

void GrubCustomizer::reset(){
	this->grublistCfg->reset();
	this->settings->clear();
}


void GrubCustomizer::showAboutDialog(){
	this->aboutDialog->show();
}


void GrubCustomizer::initRootSelector(){
	if (mountTable->getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT)){
		partitionChooser->setIsMounted(true);
		this->generateSubmountpointSelection(PARTCHOOSER_MOUNTPOINT);
	}
	else
		partitionChooser->setIsMounted(false);
	this->readPartitionInfo();
}

void GrubCustomizer::readPartitionInfo(){
	this->partitionChooser->clearPartitionSelector();
	for (DeviceDataList::iterator iter = deviceDataList->begin(); iter != deviceDataList->end(); iter++){
		this->partitionChooser->addPartitionSelectorItem(iter->first, iter->second["TYPE"], iter->second["LABEL"]);
	}
}

void GrubCustomizer::generateSubmountpointSelection(std::string const& prefix){
	this->partitionChooser->removeAllSubmountpoints();

	//create new submountpoint checkbuttons
	for (MountTable::const_iterator iter = mountTable->begin(); iter != mountTable->end(); iter++){
		if (iter->mountpoint.length() > prefix.length()
		 && iter->mountpoint != prefix + "/dev"
		 && iter->mountpoint != prefix + "/proc"
		 && iter->mountpoint != prefix + "/sys"
		) {
			this->partitionChooser->addSubmountpoint(iter->mountpoint.substr(prefix.length()), iter->isMounted);
		}
	}
}

void GrubCustomizer::mountRootFs(){
	std::string selectedDevice = this->partitionChooser->getSelectedDevice();
	partitionChooser->setIsMounted(true);
	mkdir(PARTCHOOSER_MOUNTPOINT, 0755);
	try {
		mountTable->clear(PARTCHOOSER_MOUNTPOINT);
		mountTable->mountRootFs(selectedDevice, PARTCHOOSER_MOUNTPOINT);
		this->generateSubmountpointSelection(PARTCHOOSER_MOUNTPOINT);
	}
	catch (Mountpoint::Exception e) {
		if (e == Mountpoint::MOUNT_FAILED){
			this->partitionChooser->showErrorMessage(PartitionChooser::MOUNT_FAILED);
			partitionChooser->setIsMounted(false);
		}
	}
	catch (MountTable::Exception e) {
		if (e == MountTable::MOUNT_ERR_NO_FSTAB){
			this->partitionChooser->showErrorMessage(PartitionChooser::MOUNT_ERR_NO_FSTAB);
			mountTable->getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT).umount();
			partitionChooser->setIsMounted(false);
		}
	}
}


void GrubCustomizer::umountRootFs(){
	try {
		this->mountTable->umountAll(PARTCHOOSER_MOUNTPOINT);
		this->mountTable->clear(PARTCHOOSER_MOUNTPOINT);
		partitionChooser->setIsMounted(false);
	}
	catch (Mountpoint::Exception e){
		if (e == Mountpoint::UMOUNT_FAILED)
			this->partitionChooser->showErrorMessage(PartitionChooser::UMOUNT_FAILED);
	}

	//clear list cfg dialog
	this->reset();
	this->syncListView_load();
	listCfgDlg->setLockState(1|2|8);
	this->syncSettings();
}


void GrubCustomizer::cancelPartitionChooser(){
	this->partitionChooser->hide();
	this->handleCancelResponse();
}


void GrubCustomizer::applyPartitionChooser(){
	this->partitionChooser->hide();
	listCfgDlg->setLockState(1|2|8);
	this->syncSettings();
	settingsDlg->hide();
	scriptAddDlg->hide();
	this->env.cfg_dir_prefix = PARTCHOOSER_MOUNTPOINT;
	this->init();
}

void GrubCustomizer::mountSubmountpoint(Glib::ustring const& submountpoint){
	try {
		this->mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT + submountpoint).mount();
	}
	catch (Mountpoint::Exception e){
		if (e == Mountpoint::MOUNT_FAILED){
			this->partitionChooser->showErrorMessage(PartitionChooser::SUB_MOUNT_FAILED);
		}
		this->partitionChooser->setSubmountpointSelectionState(submountpoint, false); //reset checkbox
	}
}

void GrubCustomizer::umountSubmountpoint(Glib::ustring const& submountpoint){
	try {
		this->mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT + submountpoint).umount();
	}
	catch (Mountpoint::Exception e){
		if (e == Mountpoint::UMOUNT_FAILED){
			this->partitionChooser->showErrorMessage(PartitionChooser::SUB_UMOUNT_FAILED);
		}
		this->partitionChooser->setSubmountpointSelectionState(submountpoint, true); //reset checkbox
	}
}

void GrubCustomizer::syncEntryList(){
	this->disp_sync_load();
}

void GrubCustomizer::updateSaveProgress(){
	this->disp_sync_save();
}

void GrubCustomizer::showErrorThreadDied(){
	this->disp_thread_died();
}

void GrubCustomizer::showInstallDialog(){
	installDlg->show();
}

void GrubCustomizer::installGrub(std::string device){
	this->activeThreadCount++;
	installer->threadable_install(device);
	this->activeThreadCount--;
	if (this->activeThreadCount == 0 && this->quit_requested)
		this->quit(true);
}

void GrubCustomizer::showMessageGrubInstallCompleted(std::string const& msg){
	installDlg->showMessageGrubInstallCompleted(msg);
}

void GrubCustomizer::showScriptAddDlg(){
	if (grublistCfg->repository.size() > 0){
		scriptAddDlg->clear();
		for (Repository::iterator iter = grublistCfg->repository.begin(); iter != grublistCfg->repository.end(); iter++){
			scriptAddDlg->addItem(iter->name);
		}
	}
	else
		this->listCfgDlg->showErrorMessage(gettext("No script found"));
	
	
	scriptAddDlg->show();
}

void GrubCustomizer::addScriptFromScriptAddDlg(){
	Script* script = grublistCfg->repository.getNthScript(scriptAddDlg->getSelectedEntryIndex());
	grublistCfg->proxies.push_back(Proxy(*script));
	grublistCfg->renumerate();
	
	this->syncListView_load();
	
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateScriptAddDlgPreview(){
	scriptAddDlg->clearPreview();
	Script* selectedScript = grublistCfg->repository.getNthScript(scriptAddDlg->getSelectedEntryIndex());
	if (selectedScript){
		for (Script::iterator iter = selectedScript->begin(); iter != selectedScript->end(); iter++)
			scriptAddDlg->addToPreview(iter->name);
	}
}

void GrubCustomizer::removeProxy(Proxy* p){
	this->grublistCfg->proxies.deleteProxy(p);
	this->listCfgDlg->removeProxy(p);
	this->syncListView_load();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::_rAppendRule(Rule& rule, Rule* parentRule){
	bool is_other_entries_ph = rule.type == Rule::OTHER_ENTRIES_PLACEHOLDER;
	if (rule.dataSource || is_other_entries_ph){
		Glib::ustring name;
		if (is_other_entries_ph) {
			try {
				if (rule.dataSource == NULL) {
					throw 1;
				}
				Proxy* proxy = this->grublistCfg->proxies.getProxyByRule(&rule);
				Rule* parentRule = proxy->getRuleByEntry(*rule.dataSource, proxy->rules);
				if (parentRule) {
					name = Glib::ustring::compose(gettext("(new Entries of %1)"), parentRule->outputName);
				} else {
					throw 1;
				}
			} catch (...) {
				name = gettext("(new Entries)");
			}
		} else {
			name = rule.outputName;
		}
		this->listCfgDlg->appendEntry(name, rule.isVisible, &rule, !is_other_entries_ph, parentRule);

		for (std::list<Rule>::iterator subruleIter = rule.subRules.begin(); subruleIter != rule.subRules.end(); subruleIter++) {
			this->_rAppendRule(*subruleIter, &rule);
		}
	}
}

void GrubCustomizer::syncListView_load(){
	this->listCfgDlg->setLockState(1|4);
	double progress = this->grublistCfg->getProgress();
	if (progress != 1){
		this->listCfgDlg->setProgress(progress);
		this->listCfgDlg->setStatusText(gettext("loading configuration…"));
	}
	else {
		if (this->quit_requested)
			this->quit(true);
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
				this->_rAppendRule(*ruleIter);
			}
		}
		this->grublistCfg->unlock();
	}

	if (progress == 1){
		this->updateSettingsDlg();
		this->listCfgDlg->setLockState(0);
	}
}

void GrubCustomizer::syncListView_save(){
	this->listCfgDlg->progress_pulse();
	if (this->grublistCfg->getProgress() == 1){
		if (this->grublistCfg->error_proxy_not_found){
			this->listCfgDlg->showProxyNotFoundMessage();
			this->grublistCfg->error_proxy_not_found = false;
		}
		if (this->quit_requested)
			this->quit(true);
		
		this->listCfgDlg->setLockState(0);
		
		this->listCfgDlg->hideProgressBar();
		this->listCfgDlg->setStatusText(gettext("Configuration has been saved"));
	}
	else {
		this->listCfgDlg->setStatusText(gettext("updating configuration"));
	}
}

void GrubCustomizer::die(){
	switch (this->thrownException){
		case GrublistCfg::GRUB_CFG_DIR_NOT_FOUND:
			this->listCfgDlg->showErrorMessage(
					this->env.cfg_dir+gettext(" not found. Is grub2 installed?")
			);
			break;
		case GrublistCfg::GRUB_CMD_EXEC_FAILED:
			this->listCfgDlg->showErrorMessage(
					this->env.mkconfig_cmd + gettext(" couldn't be executed successfully. You must run this as root!")
			);
			break;
	}
	this->quit(true); //exit
}

void GrubCustomizer::activateSettingsBtn(){
	this->syncSettings();
	this->listCfgDlg->setLockState(1);
}

void GrubCustomizer::quit(bool force){
	if (force){
		if (this->mountTable->getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT))
			this->mountTable->umountAll(PARTCHOOSER_MOUNTPOINT);
		Gtk::Main::quit();
	}
	else {
		int dlgResponse = this->listCfgDlg->showExitConfirmDialog(this->config_has_been_different_on_startup_but_unsaved*2 + this->modificationsUnsaved);
		if (dlgResponse == 1){
			this->save(); //starts a thread that delays the application exiting
		}

		if (dlgResponse != 0){
			if (this->activeThreadCount != 0){
				this->quit_requested = true;
				this->grublistCfg->cancelThreads();
			}
			else {
				this->quit(true); //re-run with force option
			}
		}
	}
}

void GrubCustomizer::syncProxyState(void* proxy){
	((Proxy*)proxy)->set_isExecutable(this->listCfgDlg->getProxyState(proxy));
	this->listCfgDlg->setProxyState(proxy, this->listCfgDlg->getProxyState(proxy));
	this->modificationsUnsaved = true;
	this->updateSettingsDlg();
}

void GrubCustomizer::syncRuleState(Rule* entry){
	entry->isVisible = this->listCfgDlg->getRuleState(entry);
	this->listCfgDlg->setRuleState(entry, this->listCfgDlg->getRuleState(entry));
	this->modificationsUnsaved = true;
	this->updateScriptEntry(this->grublistCfg->proxies.getProxyByRule(entry));
	this->updateSettingsDlg();
}

void GrubCustomizer::syncRuleName(Rule* entry){
	Glib::ustring oldName = entry->outputName;
	Glib::ustring newName = this->listCfgDlg->getRuleName(entry);
	if (newName == ""){
		this->listCfgDlg->showErrorMessage(gettext("Name the Entry"));
		this->listCfgDlg->setRuleName(entry, oldName);
		this->updateScriptEntry(this->grublistCfg->proxies.getProxyByRule(entry));
	}
	else {
		this->renameEntry(entry, newName);
	}
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateScriptEntry(Proxy* proxy){
	//adding (custom) if this script is modified
	if (proxy->dataSource){ //checking the Datasource before Accessing it
		Glib::ustring name = proxy->dataSource->name;
		this->listCfgDlg->setProxyName(proxy, name, false);
		if (this->grublistCfg->proxies.proxyRequired(*proxy->dataSource)){
			this->listCfgDlg->setProxyName(proxy, name, true);
		}
	}
}

void GrubCustomizer::moveRule(void* rule, int direction){
	Proxy* parentProxy = this->grublistCfg->proxies.getProxyByRule((Rule*)rule);
	//swap the contents behind the pointers
	try {
		Rule* newRule = &this->grublistCfg->moveRule((Rule*)rule, direction);

		this->syncListView_load();
		this->listCfgDlg->selectRule(newRule);
		this->updateScriptEntry(parentProxy);
		this->modificationsUnsaved = true;
	} catch (GrublistCfg::Exception e) {
		if (e == GrublistCfg::NO_MOVE_TARGET_FOUND)
			this->listCfgDlg->showErrorMessage(gettext("cannot move this entry"));
		else
			throw e;
	}
}

void GrubCustomizer::swapProxies(Proxy* a, Proxy* b){
	grublistCfg->swapProxies(a,b);
	this->listCfgDlg->swapProxies(a,b);
	this->modificationsUnsaved = true;
}


void GrubCustomizer::showRuleInfo(Rule* rule){
	if (rule && rule->dataSource)
		this->listCfgDlg->setDefaultTitleStatusText(rule->getEntryName());
	else
		this->listCfgDlg->setStatusText("");
}

void GrubCustomizer::showProxyInfo(Proxy* proxy){
	this->listCfgDlg->setStatusText("");
}

void GrubCustomizer::updateSettingsDlgResolutionList(){
	this->disp_updateSettingsDlgResolutionList();
}

void GrubCustomizer::updateSettingsDlgResolutionList_dispatched(){
	const std::list<std::string>& data = this->fbResolutionsGetter->getData();
	this->settingsDlg->clearResolutionChooser();
	for (std::list<std::string>::const_iterator iter = data.begin(); iter != data.end(); iter++)
		this->settingsDlg->addResolution(*iter);
}

void GrubCustomizer::syncSettings(){
	std::string sel = this->settingsDlg->getSelectedCustomOption();
	this->settingsDlg->removeAllSettingRows();
	for (std::list<SettingRow>::iterator iter = this->settings->begin(); iter != this->settings->end(); this->settings->iter_to_next_setting(iter)){
		this->settingsDlg->addCustomOption(iter->isActive, iter->name, iter->value);
	}
	this->settingsDlg->selectCustomOption(sel);
	std::string defEntry = this->settings->getValue("GRUB_DEFAULT");
	if (defEntry == "saved"){
		this->settingsDlg->setActiveDefEntryOption(SettingsDlg::DEF_ENTRY_SAVED);
	}
	else {
		this->settingsDlg->setActiveDefEntryOption(SettingsDlg::DEF_ENTRY_PREDEFINED);
		this->settingsDlg->setDefEntry(defEntry);
	}

	this->settingsDlg->setShowMenuCheckboxState(!this->settings->isActive("GRUB_HIDDEN_TIMEOUT", true));
	this->settingsDlg->setOsProberCheckboxState(!this->settings->isActive("GRUB_DISABLE_OS_PROBER", true));

	std::string timeoutStr;
	if (this->settingsDlg->getShowMenuCheckboxState())
		timeoutStr = this->settings->getValue("GRUB_TIMEOUT");
	else
		timeoutStr = this->settings->getValue("GRUB_HIDDEN_TIMEOUT");

	if (timeoutStr == "" || timeoutStr.find_first_not_of("0123456789") != -1) {
		timeoutStr = "10"; //default value
	}
	std::istringstream in(timeoutStr);
	int timeout;
	in >> timeout;
	this->settingsDlg->setTimeoutValue(timeout);

	this->settingsDlg->setKernelParams(this->settings->getValue("GRUB_CMDLINE_LINUX_DEFAULT"));
	this->settingsDlg->setRecoveryCheckboxState(!this->settings->isActive("GRUB_DISABLE_LINUX_RECOVERY", true));

	this->settingsDlg->setResolutionCheckboxState(this->settings->isActive("GRUB_GFXMODE", true));
	this->settingsDlg->setResolution(this->settings->getValue("GRUB_GFXMODE"));

	Glib::ustring nColor = this->settings->getValue("GRUB_COLOR_NORMAL");
	Glib::ustring hColor = this->settings->getValue("GRUB_COLOR_HIGHLIGHT");
	if (nColor != ""){
		this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_DEFAULT_FONT).selectColor(nColor.substr(0, nColor.find('/')));
		this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor(nColor.substr(nColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_DEFAULT_FONT).selectColor("light-gray");
		this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor("black");
	}
	if (hColor != ""){
		this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor(hColor.substr(0, hColor.find('/')));
		this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor(hColor.substr(hColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor("magenta");
		this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor("black");
	}

	std::string menuPicturePath = this->settings->getValue("GRUB_MENU_PICTURE");
	bool menuPicIsInGrubDir = false;
	if (menuPicturePath != "" && menuPicturePath[0] != '/'){
		menuPicturePath = env.output_config_dir + "/" + menuPicturePath;
		menuPicIsInGrubDir = true;
	}

	if (this->settings->isActive("GRUB_MENU_PICTURE") && menuPicturePath != ""){
		this->settingsDlg->setBackgroundImagePreviewPath(menuPicturePath, menuPicIsInGrubDir);
	}
	else {
		this->settingsDlg->setBackgroundImagePreviewPath("", menuPicIsInGrubDir);
	}
}

void GrubCustomizer::updateDefaultSetting(){
	if (this->settingsDlg->getActiveDefEntryOption() == SettingsDlg::DEF_ENTRY_SAVED){
		this->settings->setValue("GRUB_DEFAULT", "saved");
		this->settings->setValue("GRUB_SAVEDEFAULT", "true");
		this->settings->setIsActive("GRUB_SAVEDEFAULT", true);
	}
	else {
		this->settings->setValue("GRUB_DEFAULT", this->settingsDlg->getSelectedDefaultGrubValue());
		this->settings->setValue("GRUB_SAVEDEFAULT", "false");
	}
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateCustomSetting(std::string const& name){
	SettingsDlg::CustomOption c = this->settingsDlg->getCustomOption(name);
	this->settings->renameItem(c.old_name, c.name);
	this->settings->setValue(c.name, c.value);
	this->settings->setIsActive(c.name, c.isActive);
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::addNewCustomSettingRow(){
	std::string newSettingName = this->settings->addNewItem();
	this->syncSettings();
}
void GrubCustomizer::removeCustomSettingRow(std::string const& name){
	this->settings->removeItem(name);
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateShowMenuSetting(){
	std::string val = this->settings->getValue("GRUB_HIDDEN_TIMEOUT");
	if (val == "" || val.find_first_not_of("0123456789") != -1) {
		this->settings->setValue("GRUB_HIDDEN_TIMEOUT", "0"); //create this entry - if it has an invalid value
	}
	this->settings->setIsActive("GRUB_HIDDEN_TIMEOUT", !this->settingsDlg->getShowMenuCheckboxState());
	if (!this->settingsDlg->getShowMenuCheckboxState() && this->settingsDlg->getOsProberCheckboxState()){
		this->settingsDlg->showHiddenMenuOsProberConflictMessage();
	}
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateOsProberSetting(){
	this->settings->setValue("GRUB_DISABLE_OS_PROBER", this->settingsDlg->getOsProberCheckboxState() ? "false" : "true");
	this->settings->setIsActive("GRUB_DISABLE_OS_PROBER", !this->settingsDlg->getOsProberCheckboxState());
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateKernalParams(){
	this->settings->setValue("GRUB_CMDLINE_LINUX_DEFAULT", this->settingsDlg->getKernelParams());
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateUseCustomResolution(){
	if (this->settings->getValue("GRUB_GFXMODE") == "") {
		this->settings->setValue("GRUB_GFXMODE", "saved"); //use saved as default value (if empty)
	}
	this->settings->setIsActive("GRUB_GFXMODE", this->settingsDlg->getResolutionCheckboxState());
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::copyBackgroundImageToGrubDirectory(){
	Glib::RefPtr<Gio::File> file_src = Gio::File::create_for_path(this->settingsDlg->getBackgroundImagePath());
	Glib::RefPtr<Gio::File> file_dest = Gio::File::create_for_path(env.output_config_dir+"/"+file_src->get_basename());
	file_src->copy(file_dest, Gio::FILE_COPY_OVERWRITE);

	this->settings->setValue("GRUB_MENU_PICTURE", file_src->get_basename()); //The path isn't required when the image is in grub conf dir
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateBackgroundImage(){
	this->settings->setValue("GRUB_MENU_PICTURE", this->settingsDlg->getBackgroundImagePath());
	this->settings->setIsActive("GRUB_MENU_PICTURE", true);
	this->settings->setIsExport("GRUB_MENU_PICTURE", true);
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateColorSettings(){
	if (this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() != "" && this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor() != ""){
		this->settings->setValue("GRUB_COLOR_NORMAL", this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() + "/" + this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor());
		this->settings->setIsActive("GRUB_COLOR_NORMAL", true);
		this->settings->setIsExport("GRUB_COLOR_NORMAL", true);
	}
	if (this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() != "" && this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor() != ""){
		this->settings->setValue("GRUB_COLOR_HIGHLIGHT", this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() + "/" + this->settingsDlg->getColorChooser(SettingsDlg::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor());
		this->settings->setIsActive("GRUB_COLOR_HIGHLIGHT", true);
		this->settings->setIsExport("GRUB_COLOR_HIGHLIGHT", true);
	}
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::removeBackgroundImage(){
	this->settings->setIsActive("GRUB_MENU_PICTURE", false);
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::hideSettingsDialog(){
	this->settingsDlg->hide();
	if (this->settings->reloadRequired()){
		Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &GrubCustomizer::load), true), false);
	}
}

void GrubCustomizer::updateTimeoutSetting(){
	if (this->settingsDlg->getShowMenuCheckboxState()){
		this->settings->setValue("GRUB_TIMEOUT", Glib::ustring::format(this->settingsDlg->getTimeoutValue()));
	}
	else {
		this->settings->setValue("GRUB_HIDDEN_TIMEOUT", Glib::ustring::format(this->settingsDlg->getTimeoutValue()));
	}
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateCustomResolution(){
	this->settings->setValue("GRUB_GFXMODE", this->settingsDlg->getResolution());
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::updateGenerateRecoverySetting(){
	if (this->settings->getValue("GRUB_DISABLE_LINUX_RECOVERY") != "true") {
		this->settings->setValue("GRUB_DISABLE_LINUX_RECOVERY", "true");
	}
	this->settings->setIsActive("GRUB_DISABLE_LINUX_RECOVERY", !this->settingsDlg->getRecoveryCheckboxState());
	this->syncSettings();
	this->modificationsUnsaved = true;
}
