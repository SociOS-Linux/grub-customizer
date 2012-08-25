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

#include "MainControllerImpl.h"

MainControllerImpl::MainControllerImpl(Model_Env& env)
	: grublistCfg(NULL), view(NULL), settings(NULL),
	  settingsOnDisk(NULL),
	  savedListCfg(NULL),
	  fbResolutionsGetter(NULL), deviceDataList(NULL),
	  mountTable(NULL),
	 env(env), config_has_been_different_on_startup_but_unsaved(false),
	 modificationsUnsaved(false), quit_requested(false), activeThreadCount(0),
	 is_loading(false), contentParserFactory(NULL), currentContentParser(NULL),
	 threadController(NULL), thrownException(Model_ListCfg::GRUB_CMD_EXEC_FAILED),
	 entryNameMapper(NULL)
{
}


void MainControllerImpl::setListCfg(Model_ListCfg& grublistCfg){
	this->grublistCfg = &grublistCfg;
}
void MainControllerImpl::setView(View_Main& listCfgDlg){
	this->view = &listCfgDlg;
}

void MainControllerImpl::setSettingsManager(Model_SettingsManagerData& settings){
	this->settings = &settings;
}

void MainControllerImpl::setSettingsBuffer(Model_SettingsManagerData& settings){
	this->settingsOnDisk = &settings;
}

void MainControllerImpl::setSavedListCfg(Model_ListCfg& savedListCfg){
	this->savedListCfg = &savedListCfg;
}

void MainControllerImpl::setFbResolutionsGetter(Model_FbResolutionsGetter& fbResolutionsGetter){
	this->fbResolutionsGetter = &fbResolutionsGetter;
}

void MainControllerImpl::setDeviceDataList(Model_DeviceDataList& deviceDataList){
	this->deviceDataList = &deviceDataList;
}

void MainControllerImpl::setMountTable(Model_MountTable& mountTable){
	this->mountTable = &mountTable;
}

void MainControllerImpl::setThreadController(ThreadController& threadController) {
	this->threadController = &threadController;
}

void MainControllerImpl::setContentParserFactory(ContentParserFactory& contentParserFactory) {
	this->contentParserFactory = &contentParserFactory;
}

void MainControllerImpl::setEntryNameMapper(Mapper_EntryName& mapper) {
	this->entryNameMapper = &mapper;
}

ThreadController& MainControllerImpl::getThreadController() {
	if (this->threadController == NULL) {
		throw INCOMPLETE;
	}
	return *this->threadController;
}

Model_FbResolutionsGetter& MainControllerImpl::getFbResolutionsGetter() {
	return *this->fbResolutionsGetter;
}

void MainControllerImpl::updateList() {
	this->view->clear();

	for (std::list<Model_Proxy>::iterator iter = this->grublistCfg->proxies.begin(); iter != this->grublistCfg->proxies.end(); iter++){
		std::string name = iter->getScriptName();
		if ((name != "header" && name != "debian_theme" && name != "grub-customizer_menu_color_helper") || iter->isModified()) {
			for (std::list<Model_Rule>::iterator ruleIter = iter->rules.begin(); ruleIter != iter->rules.end(); ruleIter++){
				this->_rAppendRule(*ruleIter);
			}
		}
	}
}

void MainControllerImpl::init(){
	if   ( !grublistCfg
		or !view
		or !settings
		or !settingsOnDisk
		or !savedListCfg
		or !fbResolutionsGetter
		or !deviceDataList
		or !mountTable
		or !contentParserFactory
		or !threadController
		or !entryNameMapper
	) {
		throw INCOMPLETE;
	}
	this->log("initializing (w/o specified bootloader type)…", Logger::IMPORTANT_EVENT);
	savedListCfg->verbose = false;

	this->log("reading partition info…", Logger::EVENT);
	FILE* blkidProc = popen("blkid", "r");
	if (blkidProc){
		deviceDataList->clear();
		deviceDataList->loadData(blkidProc);
		pclose(blkidProc);
	}

	mountTable->loadData("");
	mountTable->loadData(PARTCHOOSER_MOUNTPOINT);


	this->getAllControllers().masterclass_deprecated->grubEnvSetRootDeviceName(mountTable->getEntryByMountpoint("").device);


	this->log("Loading Framebuffer resolutions (background process)", Logger::EVENT);
	//loading the framebuffer resolutions in background…
	this->getThreadController().startFramebufferResolutionLoader();

	//dir_prefix may be set by partition chooser (if not, the root partition is used)

	this->log("Finding out if this is a live CD", Logger::EVENT);
	//aufs is the virtual root fileSystem used by live cds
	if (mountTable->getEntryByMountpoint("").isLiveCdFs() && env.cfg_dir_prefix == ""){
		this->log("is live CD", Logger::INFO);
		this->showEnvEditorAction();
	} else {
		this->log("running on an installed system", Logger::INFO);
		std::list<Model_Env::Mode> modes = this->env.getAvailableModes();
		if (modes.size() == 2)
			this->view->showBurgSwitcher();
		else if (modes.size() == 1)
			this->init(modes.front());
		else if (modes.size() == 0)
			this->showEnvEditorAction();
	}
}

void MainControllerImpl::init(Model_Env::Mode mode, bool initEnv){
	this->log("initializing (w/ specified bootloader type)…", Logger::IMPORTANT_EVENT);
	if (initEnv) {
		this->env.init(mode, env.cfg_dir_prefix);
	}
	this->view->setLockState(1|4|8);
	this->view->setIsBurgMode(mode == Model_Env::BURG_MODE);
	this->view->show();
	this->view->hideBurgSwitcher();

	this->log("Checking if the config directory is clean", Logger::EVENT);
	if (this->grublistCfg->cfgDirIsClean() == false) {
		this->log("cleaning up config dir", Logger::IMPORTANT_EVENT);
		this->grublistCfg->cleanupCfgDir();
	}

	this->log("loading configuration", Logger::IMPORTANT_EVENT);
	this->getThreadController().startLoadThread(false);
}

void MainControllerImpl::reInitAction(bool burgMode) {
	Model_Env::Mode mode = burgMode ? Model_Env::BURG_MODE : Model_Env::GRUB_MODE;
	this->init(mode, false);
}

void MainControllerImpl::showEnvEditor(bool resetPartitionChooser) {
	if (this->modificationsUnsaved) {
		bool proceed = this->view->confirmUnsavedSwitch();
		if (!proceed) {
			return;
		}
	}

	this->getAllControllers().masterclass_deprecated->grubEnvsetEnvSettings(this->env.getProperties(), this->env.getRequiredProperties(), this->env.getValidProperties());
	this->view->hide();
	this->getAllControllers().masterclass_deprecated->grubEnvShow(resetPartitionChooser);
}

void MainControllerImpl::cancelBurgSwitcherAction(){
	if (!this->view->isVisible())
		this->getThreadController().stopApplication();
}

void MainControllerImpl::reloadAction(){
	this->getAllControllers().settingsController->syncAction();
	this->view->hideReloadRecommendation();
	this->view->setLockState(1|4|8);
	this->getThreadController().startLoadThread(true);
}

//threaded!
void MainControllerImpl::loadThreadedAction(bool preserveConfig){
	if (!is_loading){ //allow only one load thread at the same time!
		this->log(std::string("loading - preserveConfig: ") + (preserveConfig ? "yes" : "no"), Logger::IMPORTANT_EVENT);
		is_loading = true;
		this->activeThreadCount++;

		if (!preserveConfig){
			this->log("unsetting saved config", Logger::EVENT);
			this->grublistCfg->reset();
			this->savedListCfg->reset();
			//load the burg/grub settings file
			this->log("loading settings", Logger::IMPORTANT_EVENT);
			this->settings->load();
			this->getThreadController().enableSettings();
		} else {
			this->log("switching settings", Logger::IMPORTANT_EVENT);
			this->settingsOnDisk->load();
			this->settings->save();
		}

		try {
			this->log("loading grub list", Logger::IMPORTANT_EVENT);
			this->grublistCfg->load(preserveConfig);
			this->log("grub list completely loaded", Logger::IMPORTANT_EVENT);
		} catch (Model_ListCfg::Exception const& e){
			this->log("error while loading the grub list", Logger::ERROR);
			this->thrownException = e;
			this->getThreadController().showThreadDiedError();
			return; //cancel
		}
	
		if (!preserveConfig){
			this->log("loading saved grub list", Logger::IMPORTANT_EVENT);
			if (this->savedListCfg->loadStaticCfg()) {
				this->config_has_been_different_on_startup_but_unsaved = !this->grublistCfg->compare(*this->savedListCfg);
			} else {
				this->log("saved grub list not found", Logger::WARNING);
				this->config_has_been_different_on_startup_but_unsaved = false;
			}
		}
		if (preserveConfig){
			this->log("restoring settings", Logger::IMPORTANT_EVENT);
			this->settingsOnDisk->save();
		}
		this->activeThreadCount--;
		this->is_loading = false;
	} else {
		this->log("ignoring load request (only one load thread allowed at the same time)", Logger::WARNING);
	}
}

void MainControllerImpl::saveAction(){
	this->config_has_been_different_on_startup_but_unsaved = false;
	this->modificationsUnsaved = false; //deprecated

	this->view->setLockState(1|4|8);
	this->activeThreadCount++; //not in save_thead() to be faster set
	this->getThreadController().startSaveThread();
}

void MainControllerImpl::saveThreadedAction(){
	this->log("writing settings file", Logger::IMPORTANT_EVENT);
	this->settings->save();
	if (this->settings->color_helper_required) {
		this->grublistCfg->addColorHelper();
	}
	this->log("writing grub list configuration", Logger::IMPORTANT_EVENT);
	this->grublistCfg->save();
	this->activeThreadCount--;
}

void MainControllerImpl::renameEntry(Model_Rule* rule, std::string const& newName){
	if (rule->type != Model_Rule::PLAINTEXT) {
		if (this->settings->getValue("GRUB_DEFAULT") == rule->outputName)
			this->settings->setValue("GRUB_DEFAULT", newName);
		this->grublistCfg->renameRule(rule, newName);

		this->syncLoadStateAction();
		this->view->selectRule(rule);
	}
}

void MainControllerImpl::reset(){
	this->grublistCfg->reset();
	this->settings->clear();
}


void MainControllerImpl::showAboutAction(){
	this->getAllControllers().masterclass_deprecated->showAboutDialog();
}

void MainControllerImpl::showInstallerAction(){
	this->getAllControllers().masterclass_deprecated->showInstallDialog();
}

void MainControllerImpl::showMessageGrubInstallCompleted(std::string const& msg){
	this->getAllControllers().masterclass_deprecated->showMessageGrubInstallCompleted(msg);
}

void MainControllerImpl::showEntryEditorAction(void* rule) {
	this->getAllControllers().entryEditController->showAction(rule);
}

void MainControllerImpl::showEntryCreatorAction() {
	this->getAllControllers().entryEditController->showCreatorAction();
}

void MainControllerImpl::_rAppendRule(Model_Rule& rule, Model_Rule* parentRule){
	bool is_other_entries_ph = rule.type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER;
	bool is_plaintext = rule.dataSource && rule.dataSource->type == Model_Entry::PLAINTEXT;
	bool is_submenu = rule.type == Model_Rule::SUBMENU;

	if (rule.dataSource || is_submenu){
		std::string name = this->entryNameMapper->map(rule.dataSource, rule.outputName);

		bool isSubmenu = rule.type == Model_Rule::SUBMENU;
		std::string scriptName = "", defaultName = "";
		if (rule.dataSource) {
			scriptName = this->grublistCfg->repository.getScriptByEntry(*rule.dataSource)->name;
			if (!is_other_entries_ph && !is_plaintext) {
				defaultName = rule.dataSource->name;
			}
		}
		bool isEditable = rule.type == Model_Rule::NORMAL || rule.type == Model_Rule::PLAINTEXT;
		bool isModified = rule.dataSource && rule.dataSource->isModified;
		if (rule.isVisible) {
			// parse content to show additional informations
			std::map<std::string, std::string> options;
			if (rule.dataSource) {
				try {
					options = this->contentParserFactory->create(rule.dataSource->content)->getOptions();
					if (options.find("partition_uuid") != options.end()) {
						// add device path
						for (DeviceDataList_Iface::const_iterator iter = deviceDataList->begin(); iter != deviceDataList->end(); iter++) {
							if (iter->second.at("UUID") == options["partition_uuid"]) {
								options["_deviceName"] = iter->first;
								break;
							}
						}
					}
				} catch (ContentParserFactory::Exception const& e) {
					if (e != ContentParserFactory::CREATION_FAILED) {
						throw e;
					}
				}
			}

			this->view->appendEntry(name, &rule, is_other_entries_ph || is_plaintext, isSubmenu, scriptName, defaultName, isEditable, isModified, options, parentRule);

			for (std::list<Model_Rule>::iterator subruleIter = rule.subRules.begin(); subruleIter != rule.subRules.end(); subruleIter++) {
				this->_rAppendRule(*subruleIter, &rule);
			}
		}
	}
}

void MainControllerImpl::dieAction(){
	this->is_loading = false;
	this->activeThreadCount = 0;
	bool showEnvSettings = false;
	if (this->thrownException == Model_ListCfg::GRUB_CMD_EXEC_FAILED){
		showEnvSettings = this->view->askForEnvironmentSettings(this->env.mkconfig_cmd, this->grublistCfg->getGrubErrorMessage());
	}
	if (showEnvSettings) {
		this->showEnvEditor();
	} else {
		this->exitAction(true); //exit
	}
}

void MainControllerImpl::exitAction(bool force){
	if (force){
		if (this->mountTable->getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT))
			this->mountTable->umountAll(PARTCHOOSER_MOUNTPOINT);
		this->getThreadController().stopApplication();
	}
	else {
		int dlgResponse = this->view->showExitConfirmDialog(this->config_has_been_different_on_startup_but_unsaved*2 + this->modificationsUnsaved);
		if (dlgResponse == 1){
			this->saveAction(); //starts a thread that delays the application exiting
		}

		if (dlgResponse != 0){
			if (this->activeThreadCount != 0){
				this->quit_requested = true;
				this->grublistCfg->cancelThreads();
			}
			else {
				this->exitAction(true); //re-run with force option
			}
		}
	}
}

void MainControllerImpl::removeRulesAction(std::list<void*> entries){
	std::map<Model_Proxy*, void*> emptyProxies;
	for (std::list<void*>::iterator iter = entries.begin(); iter != entries.end(); iter++) {
		Model_Rule* rule = static_cast<Model_Rule*>(*iter);
		rule->setVisibility(false);
		if (!this->grublistCfg->proxies.getProxyByRule(rule)->hasVisibleRules()) {
			emptyProxies[this->grublistCfg->proxies.getProxyByRule(rule)] = NULL;
		}
	}

	for (std::map<Model_Proxy*, void*>::iterator iter = emptyProxies.begin(); iter != emptyProxies.end(); iter++) {
		this->grublistCfg->proxies.deleteProxy(iter->first);
		this->log("proxy removed", Logger::INFO);
	}

	this->syncLoadStateAction();
	this->modificationsUnsaved = true;
	this->getAllControllers().settingsController->updateSettingsDataAction();
}


void MainControllerImpl::renameRuleAction(void* entry, std::string const& newText){
	Model_Rule* entry2 = (Model_Rule*)entry;
	std::string oldName = entry2->outputName;
//	std::string newName = this->view->getRuleName(entry);
	if (newText == ""){
		this->view->showErrorMessage(gettext("Name the Entry"));
		this->view->setRuleName(entry, oldName);
	}
	else {
		this->renameEntry(entry2, newText);
	}
	this->modificationsUnsaved = true;
}


void MainControllerImpl::moveAction(std::list<void*> rules, int direction){
	try {
		assert(direction == -1 || direction == 1);

		int ruleCount = rules.size();
		Model_Rule* rulePtr = static_cast<Model_Rule*>(direction == -1 ? rules.front() : rules.back());
		for (int i = 0; i < ruleCount; i++) {
			rulePtr = &this->grublistCfg->moveRule(rulePtr, direction);
			if (i < ruleCount - 1) {
				bool isEndOfList = false;
				bool targetFound = false;
				try {
					rulePtr = &*this->grublistCfg->proxies.getNextVisibleRule(rulePtr, -direction);
				} catch (Model_Proxylist::Exception e) {
					isEndOfList = true;
					rulePtr = this->grublistCfg->proxies.getProxyByRule(rulePtr)->getParentRule(rulePtr);
				}
				if (!isEndOfList && rulePtr->type == Model_Rule::SUBMENU) {
					rulePtr = direction == -1 ? &rulePtr->subRules.front() : &rulePtr->subRules.back();
					if (rulePtr->isVisible) {
						targetFound = true;
					}
				}

				if (!targetFound) {
					rulePtr = &*this->grublistCfg->proxies.getNextVisibleRule(rulePtr, -direction);
				}
			}
		}

		std::list<void*> movedRules;
		movedRules.push_back(rulePtr);
		for (int i = 1; i < ruleCount; i++) {
			movedRules.push_back(&*this->grublistCfg->proxies.getNextVisibleRule(static_cast<Model_Rule*>(movedRules.back()), direction));
		}

		this->syncLoadStateAction();
		this->view->selectRules(movedRules);
		this->modificationsUnsaved = true;
	} catch (Model_ListCfg::Exception const& e) {
		if (e == Model_ListCfg::NO_MOVE_TARGET_FOUND)
			this->view->showErrorMessage(gettext("cannot move this entry"));
		else
			throw e;
	}
}


void MainControllerImpl::createSubmenuAction(std::list<void*> childItems) {
	Model_Rule* firstRule = static_cast<Model_Rule*>(childItems.front());
	Model_Rule* newItem = this->grublistCfg->createSubmenu(firstRule);
	this->syncLoadStateAction();
	this->moveAction(childItems, -1);
	this->view->selectRule(newItem, true);
}

void MainControllerImpl::removeSubmenuAction(std::list<void*> childItems) {
	Model_Rule* firstItem = this->grublistCfg->splitSubmenu(static_cast<Model_Rule*>(childItems.front()));
	std::list<void*> movedRules;
	movedRules.push_back(firstItem);
	for (int i = 1; i < childItems.size(); i++) {
		movedRules.push_back(&*this->grublistCfg->proxies.getNextVisibleRule(static_cast<Model_Rule*>(movedRules.back()), 1));
	}

	this->moveAction(movedRules, -1);
}

void MainControllerImpl::revertAction() {
	int remaining = this->grublistCfg->proxies.size();
	while (remaining) {
		this->grublistCfg->proxies.deleteProxy(&this->grublistCfg->proxies.front());
		assert(this->grublistCfg->proxies.size() < remaining); // make sure that the proxy has really been deleted to prevent an endless loop
		remaining = this->grublistCfg->proxies.size();
	}
	int i = 50; // unknown scripts starting at position 50
	for (std::list<Model_Script>::iterator iter = this->grublistCfg->repository.begin(); iter != this->grublistCfg->repository.end(); iter++) {
		Model_Proxy newProxy(*iter);
		if (iter->name == "header") {
			newProxy.index = 0;
		} else if (iter->name == "debian_theme") {
			newProxy.index = 5;
		} else if (iter->name == "grub-customizer_menu_color_helper") {
			newProxy.index = 6;
		} else if (iter->name == "linux") {
			newProxy.index = 10;
		} else if (iter->name == "linux_xen" || iter->name == "memtest86+") {
			newProxy.index = 20;
		} else if (iter->name == "os-prober") {
			newProxy.index = 30;
		} else if (iter->name == "custom" && iter->isCustomScript) {
			newProxy.index = 40;
		} else if (iter->name == "custom" && !iter->isCustomScript) {
			newProxy.index = 41;
		} else {
			newProxy.index = i++;
		}

		this->grublistCfg->proxies.push_back(newProxy);
	}
	this->grublistCfg->proxies.sort();
	this->syncLoadStateAction();
}

void MainControllerImpl::showInfoAction(void* rule){
	if (rule == NULL) {
		return;
	}
	Model_Rule* rule2 = (Model_Rule*)rule;
	if (rule2 && rule2->dataSource)
		this->view->setDefaultTitleStatusText(rule2->getEntryName());
	else
		this->view->setStatusText("");
}

void MainControllerImpl::showProxyInfo(Model_Proxy* proxy){
	this->view->setStatusText("");
}


void MainControllerImpl::syncLoadStateThreadedAction() {
	this->getThreadController().syncEntryList();
}
void MainControllerImpl::syncSaveStateThreadedAction() {
	this->getThreadController().updateSaveProgress();
}

void MainControllerImpl::syncSaveStateAction(){
	this->log("running MainControllerImpl::syncListView_save", Logger::INFO);
	this->view->progress_pulse();
	if (this->grublistCfg->getProgress() == 1){
		if (this->grublistCfg->error_proxy_not_found){
			this->view->showProxyNotFoundMessage();
			this->grublistCfg->error_proxy_not_found = false;
		}
		if (this->quit_requested)
			this->exitAction(true);

		this->view->setLockState(0);

		this->view->hideProgressBar();
		this->view->setStatusText(gettext("Configuration has been saved"));

		this->updateList();
	}
	else {
		this->view->setStatusText(gettext("updating configuration"));
	}
	this->log("MainControllerImpl::syncListView_save completed", Logger::INFO);
}

void MainControllerImpl::syncLoadStateAction() {
	this->log("running MainControllerImpl::syncListView_load", Logger::INFO);
	this->view->setLockState(1|4);
	double progress = this->grublistCfg->getProgress();
	if (progress != 1) {
		this->view->setProgress(progress);
		this->view->setStatusText(this->grublistCfg->getProgress_name(), this->grublistCfg->getProgress_pos(), this->grublistCfg->getProgress_max());
	} else {
		if (this->quit_requested) {
			this->exitAction(true);
		}
		this->view->hideProgressBar();
		this->view->setStatusText("");
	}

	//if grubConfig is locked, it will be cancelled as early as possible
	if (this->grublistCfg->lock_if_free()) {
		this->updateList();
		this->grublistCfg->unlock();
	}

	if (progress == 1){
		this->getAllControllers().settingsController->updateSettingsDataAction();

		this->view->setTrashCounter(this->grublistCfg->getRemovedEntries().size());
		this->view->setLockState(0);
	}
	this->log("MainControllerImpl::syncListView_load completed", Logger::INFO);
}

void MainControllerImpl::showSettingsAction() {
	this->getAllControllers().settingsController->showAction(env.burgMode);
}

void MainControllerImpl::showEnvEditorAction(bool resetPartitionChooser) {
	if (this->modificationsUnsaved) {
		bool proceed = this->view->confirmUnsavedSwitch();
		if (!proceed) {
			return;
		}
	}

	this->view->hide();

	this->getAllControllers().masterclass_deprecated->showEnvEditor();
}

void MainControllerImpl::showTrashAction() {
	this->getAllControllers().masterclass_deprecated->showEntryAddDlg();
}

void MainControllerImpl::initModeAction(bool burgChosen) {
	this->init(burgChosen ? Model_Env::BURG_MODE : Model_Env::GRUB_MODE);
}

void MainControllerImpl::addEntriesAction(std::list<void*> entries) {
	std::list<void*> addedRules;
	for (std::list<void*>::iterator iter = entries.begin(); iter != entries.end(); iter++) {
		Model_Entry* entry = static_cast<Model_Entry*>(*iter);
		addedRules.push_back(this->grublistCfg->addEntry(*entry));
	}

	this->syncLoadStateAction();

	this->view->selectRules(addedRules);

	this->modificationsUnsaved = true;
}

void MainControllerImpl::activateSettingsAction() {
	this->view->setLockState(1);
	this->getAllControllers().settingsController->syncAction();
}


void MainControllerImpl::showReloadRecommendationAction() {
	this->view->showReloadRecommendation();
}

void MainControllerImpl::selectRulesAction(std::list<void*> rules) {
	this->view->selectRules(rules);
}

void MainControllerImpl::selectRuleAction(void* rule, bool startEdit) {
	this->view->selectRule(rule, startEdit);
}
