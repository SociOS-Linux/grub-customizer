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
	  installer(NULL), installDlg(NULL), settingsOnDisk(NULL), entryAddDlg(NULL),
	  entryEditDlg(NULL),
	  savedListCfg(NULL),
	  fbResolutionsGetter(NULL), deviceDataList(NULL),
	  mountTable(NULL), aboutDialog(NULL),
	 env(env), config_has_been_different_on_startup_but_unsaved(false),
	 modificationsUnsaved(false), quit_requested(false), activeThreadCount(0),
	 is_loading(false), contentParserFactory(NULL), currentContentParser(NULL),
	 grubEnvEditor(NULL)
{
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
void GrubCustomizer::setScriptAddDlg(EntryAddDlg& scriptAddDlg){
	this->entryAddDlg = &scriptAddDlg;
}

void GrubCustomizer::setEntryEditDlg(EntryEditDlg& entryEditDlg) {
	this->entryEditDlg = &entryEditDlg;
}

void GrubCustomizer::setSavedListCfg(GrublistCfg& savedListCfg){
	this->savedListCfg = &savedListCfg;
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

void GrubCustomizer::setThreadController(ThreadController& threadController) {
	this->threadController = &threadController;
}

void GrubCustomizer::setContentParserFactory(ContentParserFactory& contentParserFactory) {
	this->contentParserFactory = &contentParserFactory;
}

void GrubCustomizer::setGrubEnvEditor(GrubEnvEditor& envEditor) {
	this->grubEnvEditor = &envEditor;
}

ThreadController& GrubCustomizer::getThreadController() {
	if (this->threadController == NULL) {
		throw INCOMPLETE;
	}
	return *this->threadController;
}

FbResolutionsGetter& GrubCustomizer::getFbResolutionsGetter() {
	return *this->fbResolutionsGetter;
}

void GrubCustomizer::updateList() {
	this->listCfgDlg->clear();

	for (std::list<Proxy>::iterator iter = this->grublistCfg->proxies.begin(); iter != this->grublistCfg->proxies.end(); iter++){
		std::string name = iter->getScriptName();
		if (name != "header" && name != "debian_theme" && name != "grub-customizer_menu_color_helper" || iter->isModified()) {
			for (std::list<Rule>::iterator ruleIter = iter->rules.begin(); ruleIter != iter->rules.end(); ruleIter++){
				this->_rAppendRule(*ruleIter);
			}
		}
	}
}

void GrubCustomizer::updateSettingsDlg(){
	std::list<EntryTitleListItem> entryTitles = this->grublistCfg->proxies.generateEntryTitleList();
	std::list<std::string> labelListToplevel  = this->grublistCfg->proxies.getToplevelEntryTitles();

	this->settingsDlg->clearDefaultEntryChooser();
	for (std::list<EntryTitleListItem>::iterator iter = entryTitles.begin(); iter != entryTitles.end(); iter++) {
		this->settingsDlg->addEntryToDefaultEntryChooser(iter->labelPathValue, iter->labelPathLabel, iter->numericPathValue, iter->numericPathLabel);
	}

	this->settingsDlg->setPreviewEntryTitles(labelListToplevel);

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
		or !entryAddDlg
		or !savedListCfg
		or !fbResolutionsGetter
		or !deviceDataList
		or !mountTable
		or !aboutDialog
		or !entryEditDlg
		or !contentParserFactory
		or !grubEnvEditor
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


	grubEnvEditor->setRootDeviceName(mountTable->getEntryByMountpoint("").device);


	this->log("Loading Framebuffer resolutions (background process)", Logger::EVENT);
	//loading the framebuffer resolutions in background…
	this->getThreadController().startFramebufferResolutionLoader();

	//dir_prefix may be set by partition chooser (if not, the root partition is used)

	this->log("Finding out if this is a live CD", Logger::EVENT);
	//aufs is the virtual root fileSystem used by live cds
	if (mountTable->getEntryByMountpoint("").isLiveCdFs() && env.cfg_dir_prefix == ""){
		this->log("is live CD", Logger::INFO);
		this->showEnvEditor();
	} else {
		this->log("running on an installed system", Logger::INFO);
		std::list<GrubEnv::Mode> modes = this->env.getAvailableModes();
		if (modes.size() == 2)
			this->listCfgDlg->showBurgSwitcher();
		else if (modes.size() == 1)
			this->init(modes.front());
		else if (modes.size() == 0)
			this->showEnvEditor();
	}
}

void GrubCustomizer::init(GrubEnv::Mode mode, bool initEnv){
	this->log("initializing (w/ specified bootloader type)…", Logger::IMPORTANT_EVENT);
	if (initEnv) {
		this->env.init(mode, env.cfg_dir_prefix);
	}
	this->listCfgDlg->setLockState(1|4|8);
	this->listCfgDlg->setIsBurgMode(mode == GrubEnv::BURG_MODE);
	this->listCfgDlg->show();
	this->listCfgDlg->hideBurgSwitcher();

	this->log("Checking if the config directory is clean", Logger::EVENT);
	if (this->grublistCfg->cfgDirIsClean() == false) {
		this->log("cleaning up config dir", Logger::IMPORTANT_EVENT);
		this->grublistCfg->cleanupCfgDir();
	}

	this->log("loading configuration", Logger::IMPORTANT_EVENT);
	this->getThreadController().startLoadThread(false);
}

void GrubCustomizer::showEnvEditor(bool resetPartitionChooser) {
	this->grubEnvEditor->setEnvSettings(this->env.getProperties(), this->env.getRequiredProperties(), this->env.getValidProperties());
	this->listCfgDlg->hide();
	this->grubEnvEditor->show(resetPartitionChooser);
}

void GrubCustomizer::handleCancelResponse(){
	if (!listCfgDlg->isVisible())
		this->getThreadController().stopApplication();
}

void GrubCustomizer::showSettingsDlg(){
	this->settingsDlg->show(env.burgMode);
}

void GrubCustomizer::reload(){
	this->syncSettings();
	this->listCfgDlg->setLockState(1|4|8);
	this->getThreadController().startLoadThread(true);
}

//threaded!
void GrubCustomizer::load(bool preserveConfig){
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
		} catch (GrublistCfg::Exception e){
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

void GrubCustomizer::save(){
	this->config_has_been_different_on_startup_but_unsaved = false;
	this->modificationsUnsaved = false; //deprecated

	this->listCfgDlg->setLockState(1|4|8);
	this->activeThreadCount++; //not in save_thead() to be faster set
	this->getThreadController().startSaveThread();
}

void GrubCustomizer::save_thread(){
	this->log("writing settings file", Logger::IMPORTANT_EVENT);
	this->settings->save();
	if (this->settings->color_helper_required) {
		this->grublistCfg->addColorHelper();
	}
	this->log("writing grub list configuration", Logger::IMPORTANT_EVENT);
	this->grublistCfg->save();
	this->activeThreadCount--;
}

void GrubCustomizer::renameEntry(Rule* rule, std::string const& newName){
	if (rule->type != Rule::PLAINTEXT) {
		if (this->settings->getValue("GRUB_DEFAULT") == rule->outputName)
			this->settings->setValue("GRUB_DEFAULT", newName);
		this->grublistCfg->renameRule(rule, newName);

		this->syncListView_load();
		this->listCfgDlg->selectRule(rule);
	}
}

void GrubCustomizer::reset(){
	this->grublistCfg->reset();
	this->settings->clear();
}


void GrubCustomizer::showAboutDialog(){
	this->aboutDialog->show();
}

void GrubCustomizer::applyEntryEditorModifications() {
	Rule* rulePtr = static_cast<Rule*>(this->entryEditDlg->getRulePtr());
	assert(rulePtr != NULL);
	Script* script = this->grublistCfg->repository.getScriptByEntry(*rulePtr->dataSource);
	assert(script != NULL);

	if (!script->isCustomScript) {
		script = this->grublistCfg->repository.getCustomScript();
		script->entries().push_back(*rulePtr->dataSource);

		Rule ruleCopy = *rulePtr;
		rulePtr->setVisibility(false);
		ruleCopy.dataSource = &script->entries().back();
		Proxy* proxy = this->grublistCfg->proxies.getProxyByRule(rulePtr);
		std::list<Rule>& ruleList = proxy->getRuleList(proxy->getParentRule(rulePtr));

		Rule dummySubmenu(Rule::SUBMENU, std::list<std::string>(), "DUMMY", true);
		dummySubmenu.subRules.push_back(ruleCopy);
		std::list<Rule>::iterator iter = ruleList.insert(proxy->getListIterator(*rulePtr, ruleList), dummySubmenu);

		Rule& insertedRule = iter->subRules.back();
		rulePtr = &this->grublistCfg->moveRule(&insertedRule, -1);

		std::list<Proxy*> proxies = this->grublistCfg->proxies.getProxiesByScript(*script);
		for (std::list<Proxy*>::iterator proxyIter = proxies.begin(); proxyIter != proxies.end(); proxyIter++) {
			if (!(*proxyIter)->getRuleByEntry(*rulePtr->dataSource, (*proxyIter)->rules, rulePtr->type)) {
				(*proxyIter)->rules.push_back(Rule(*rulePtr->dataSource, false, *script));
			}
		}
	}

	std::string newCode = this->entryEditDlg->getSourcecode();
	rulePtr->dataSource->content = newCode;
	rulePtr->dataSource->isModified = true;

	this->modificationsUnsaved = true;

	this->syncListView_load();

	this->listCfgDlg->selectRule(rulePtr);

	this->currentContentParser = NULL;
}


void GrubCustomizer::generateSubmountpointSelection(std::string const& prefix){
	this->grubEnvEditor->removeAllSubmountpoints();

	//create new submountpoint checkbuttons
	for (MountTable::const_iterator iter = mountTable->begin(); iter != mountTable->end(); iter++){
		if (iter->mountpoint.length() > prefix.length() && iter->mountpoint.substr(0, prefix.length()) == prefix
		 && iter->mountpoint != prefix + "/dev"
		 && iter->mountpoint != prefix + "/proc"
		 && iter->mountpoint != prefix + "/sys"
		) {
			this->grubEnvEditor->addSubmountpoint(iter->mountpoint.substr(prefix.length()), iter->isMounted);
		}
	}
}

void GrubCustomizer::switchPartition(std::string const& newPartition) {
	if (this->mountTable->getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT).isMounted) {
		this->mountTable->umountAll(PARTCHOOSER_MOUNTPOINT);
		this->mountTable->clear(PARTCHOOSER_MOUNTPOINT);
	}
	this->grubEnvEditor->removeAllSubmountpoints();
	std::string selectedDevice = newPartition;
	if (newPartition != "") {
		mkdir(PARTCHOOSER_MOUNTPOINT, 0755);
		try {
			mountTable->clear(PARTCHOOSER_MOUNTPOINT);
			mountTable->mountRootFs(selectedDevice, PARTCHOOSER_MOUNTPOINT);
			this->env.init(env.burgMode ? GrubEnv::BURG_MODE : GrubEnv::GRUB_MODE, PARTCHOOSER_MOUNTPOINT);
			this->generateSubmountpointSelection(PARTCHOOSER_MOUNTPOINT);
			this->showEnvEditor();
		}
		catch (Mountpoint::Exception e) {
			if (e == Mountpoint::MOUNT_FAILED){
				this->grubEnvEditor->showErrorMessage(GrubEnvEditor::MOUNT_FAILED);
				this->switchPartition("");
			}
		}
		catch (MountTable::Exception e) {
			if (e == MountTable::MOUNT_ERR_NO_FSTAB){
				this->grubEnvEditor->showErrorMessage(GrubEnvEditor::MOUNT_ERR_NO_FSTAB);
				mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT).umount();
				this->switchPartition("");
			}
		}
	} else {
		this->env.init(env.burgMode ? GrubEnv::BURG_MODE : GrubEnv::GRUB_MODE, selectedDevice);
		this->showEnvEditor(true);
	}
}

void GrubCustomizer::switchBootloaderType(int newTypeIndex) {
	this->env.init(newTypeIndex == 0 ? GrubEnv::GRUB_MODE : GrubEnv::BURG_MODE, this->env.cfg_dir_prefix);
	this->showEnvEditor();
}

void GrubCustomizer::updateGrubEnvOptions() {
	this->env.setProperties(this->grubEnvEditor->getEnvSettings());
	this->showEnvEditor();
}

void GrubCustomizer::applyEnvEditor(bool saveConfig){
	listCfgDlg->setLockState(1|2|8);
	this->syncSettings();
	settingsDlg->hide();
	entryAddDlg->hide();
	GrubEnv::Mode mode = this->grubEnvEditor->getBootloaderType() == 0 ? GrubEnv::GRUB_MODE : GrubEnv::BURG_MODE;
	grubEnvEditor->hide();

	if (saveConfig) {
		this->env.save();
	}
	this->init(mode, false);
}


void GrubCustomizer::mountSubmountpoint(std::string const& submountpoint){
	try {
		this->mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT + submountpoint).mount();
	}
	catch (Mountpoint::Exception e){
		if (e == Mountpoint::MOUNT_FAILED){
			this->grubEnvEditor->showErrorMessage(GrubEnvEditor::SUB_MOUNT_FAILED);
		}
		this->grubEnvEditor->setSubmountpointSelectionState(submountpoint, false);
		this->grubEnvEditor->show();
	}
}

void GrubCustomizer::umountSubmountpoint(std::string const& submountpoint){
	try {
		this->mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT + submountpoint).umount();
	}
	catch (Mountpoint::Exception e){
		if (e == Mountpoint::UMOUNT_FAILED){
			this->grubEnvEditor->showErrorMessage(GrubEnvEditor::SUB_UMOUNT_FAILED);
		}
		this->grubEnvEditor->setSubmountpointSelectionState(submountpoint, true);
		this->grubEnvEditor->show();
	}
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

void GrubCustomizer::showEntryAddDlg(){
	entryAddDlg->clear();

	std::list<Entry*> removedEntries = this->grublistCfg->getRemovedEntries();
	for (std::list<Entry*>::iterator iter = removedEntries.begin(); iter != removedEntries.end(); iter++) {
		Script* script = this->grublistCfg->repository.getScriptByEntry(**iter);

		std::string name = (*iter)->name;
		name = this->_mapEntryName(&**iter, name, script->name);

		entryAddDlg->addItem(name, (*iter)->type != Entry::MENUENTRY, script->name, *iter);
	}

	entryAddDlg->show();
}

void GrubCustomizer::showEntryEditDlg(void* rule) {
	this->entryEditDlg->setRulePtr(rule);
	this->entryEditDlg->setSourcecode(((Rule*)rule)->dataSource->content);
	this->syncEntryEditDlg(false);
	this->entryEditDlg->show();
}

void GrubCustomizer::syncEntryEditDlg(bool useOptionsAsSource) {
	try {
		if (useOptionsAsSource) {
			assert(this->currentContentParser != NULL);
			this->currentContentParser->setOptions(this->entryEditDlg->getOptions());
			this->entryEditDlg->setSourcecode(this->currentContentParser->buildSource());
		} else {
			this->currentContentParser = this->contentParserFactory->create(this->entryEditDlg->getSourcecode());
			this->entryEditDlg->setOptions(this->currentContentParser->getOptions());
		}
		this->entryEditDlg->showOptions();
	} catch (ContentParserFactory::Exception e) {
		this->entryEditDlg->hideOptions();
	}
}

void GrubCustomizer::addEntryFromEntryAddDlg(){
	std::list<void*> entries = entryAddDlg->getSelectedEntries();
	std::list<void*> addedRules;
	for (std::list<void*>::iterator iter = entries.begin(); iter != entries.end(); iter++) {
		Entry* entry = static_cast<Entry*>(*iter);
		addedRules.push_back(this->grublistCfg->addEntry(*entry));
	}

	this->syncListView_load();

	this->listCfgDlg->selectRules(addedRules);
	
	this->modificationsUnsaved = true;
}

std::string GrubCustomizer::_mapEntryName(Entry const* sourceEntry, std::string const& defaultName, std::string const& scriptName) {
	std::string name;
	bool is_other_entries_ph = sourceEntry ? sourceEntry->type == Entry::SUBMENU || sourceEntry->type == Entry::SCRIPT_ROOT : false;
	bool is_plaintext = sourceEntry ? sourceEntry->type == Entry::PLAINTEXT : false;
	if (is_other_entries_ph) {
		try {
			if (sourceEntry->type == Entry::SCRIPT_ROOT) {
				throw 1;
			}
			name = this->listCfgDlg->createNewEntriesPlaceholderString(sourceEntry->name, scriptName);
		} catch (...) {
			name = this->listCfgDlg->createNewEntriesPlaceholderString("", scriptName);
		}
	} else if (is_plaintext) {
		name = this->listCfgDlg->createPlaintextString(scriptName);
	} else {
		name = defaultName;
	}
	return name;
}

void GrubCustomizer::_rAppendRule(Rule& rule, Rule* parentRule){
	bool is_other_entries_ph = rule.type == Rule::OTHER_ENTRIES_PLACEHOLDER;
	bool is_plaintext = rule.dataSource && rule.dataSource->type == Entry::PLAINTEXT;
	bool is_submenu = rule.type == Rule::SUBMENU;

	if (rule.dataSource || is_submenu){
		std::string name = this->_mapEntryName(rule.dataSource, rule.outputName);

		bool isSubmenu = rule.type == Rule::SUBMENU;
		std::string scriptName = "", defaultName = "";
		if (rule.dataSource) {
			scriptName = this->grublistCfg->repository.getScriptByEntry(*rule.dataSource)->name;
			if (!is_other_entries_ph && !is_plaintext) {
				defaultName = rule.dataSource->name;
			}
		}
		bool isEditable = rule.type == Rule::NORMAL || rule.type == Rule::PLAINTEXT;
		bool isModified = rule.dataSource && rule.dataSource->isModified;
		if (rule.isVisible) {
			this->listCfgDlg->appendEntry(name, &rule, is_other_entries_ph || is_plaintext, isSubmenu, scriptName, defaultName, isEditable, isModified, parentRule);

			for (std::list<Rule>::iterator subruleIter = rule.subRules.begin(); subruleIter != rule.subRules.end(); subruleIter++) {
				this->_rAppendRule(*subruleIter, &rule);
			}
		}
	}
}

void GrubCustomizer::syncListView_load() {
	this->log("running GrubCustomizer::syncListView_load", Logger::INFO);
	this->listCfgDlg->setLockState(1|4);
	double progress = this->grublistCfg->getProgress();
	if (progress != 1) {
		this->listCfgDlg->setProgress(progress);
		this->listCfgDlg->setStatusText(gettext("loading configuration…"));
	} else {
		if (this->quit_requested) {
			this->quit(true);
		}
		this->listCfgDlg->hideProgressBar();
		this->listCfgDlg->setStatusText("");
	}
	
	//if grubConfig is locked, it will be cancelled as early as possible
	if (this->grublistCfg->lock_if_free()) {
		this->updateList();
		this->grublistCfg->unlock();
	}

	if (progress == 1){
		this->updateSettingsDlg();
		this->listCfgDlg->setTrashCounter(this->grublistCfg->getRemovedEntries().size());
		this->listCfgDlg->setLockState(0);
	}
	this->log("GrubCustomizer::syncListView_load completed", Logger::INFO);
}

void GrubCustomizer::syncListView_save(){
	this->log("running GrubCustomizer::syncListView_save", Logger::INFO);
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

		this->updateList();
	}
	else {
		this->listCfgDlg->setStatusText(gettext("updating configuration"));
	}
	this->log("GrubCustomizer::syncListView_save completed", Logger::INFO);
}

void GrubCustomizer::die(){
	this->is_loading = false;
	this->activeThreadCount = 0;
	bool showEnvSettings = false;
	if (this->thrownException == GrublistCfg::GRUB_CMD_EXEC_FAILED){
		showEnvSettings = this->listCfgDlg->askForEnvironmentSettings(this->env.mkconfig_cmd, this->grublistCfg->getGrubErrorMessage());
	}
	if (showEnvSettings) {
		this->showEnvEditor();
	} else {
		this->quit(true); //exit
	}
}

void GrubCustomizer::activateSettingsBtn(){
	this->syncSettings();
	this->listCfgDlg->setLockState(1);
}

void GrubCustomizer::quit(bool force){
	if (force){
		if (this->mountTable->getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT))
			this->mountTable->umountAll(PARTCHOOSER_MOUNTPOINT);
		this->getThreadController().stopApplication();
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

void GrubCustomizer::removeRules(std::list<void*> entries){
	std::map<Proxy*, void*> emptyProxies;
	for (std::list<void*>::iterator iter = entries.begin(); iter != entries.end(); iter++) {
		Rule* rule = static_cast<Rule*>(*iter);
		rule->setVisibility(false);
		if (!this->grublistCfg->proxies.getProxyByRule(rule)->hasVisibleRules()) {
			emptyProxies[this->grublistCfg->proxies.getProxyByRule(rule)] = NULL;
		}
	}

	for (std::map<Proxy*, void*>::iterator iter = emptyProxies.begin(); iter != emptyProxies.end(); iter++) {
		this->grublistCfg->proxies.deleteProxy(iter->first);
		this->log("proxy removed", Logger::INFO);
	}

	this->syncListView_load();
	this->modificationsUnsaved = true;
	this->updateSettingsDlg();
}


void GrubCustomizer::updateRuleName(Rule* entry, std::string const& newText){
	std::string oldName = entry->outputName;
//	std::string newName = this->listCfgDlg->getRuleName(entry);
	if (newText == ""){
		this->listCfgDlg->showErrorMessage(gettext("Name the Entry"));
		this->listCfgDlg->setRuleName(entry, oldName);
	}
	else {
		this->renameEntry(entry, newText);
	}
	this->modificationsUnsaved = true;
}


void GrubCustomizer::moveRules(std::list<void*> rules, int direction){
	try {
		assert(direction == -1 || direction == 1);

		int ruleCount = rules.size();
		Rule* rulePtr = static_cast<Rule*>(direction == -1 ? rules.front() : rules.back());
		for (int i = 0; i < ruleCount; i++) {
			rulePtr = &this->grublistCfg->moveRule(rulePtr, direction);
			if (i < ruleCount - 1) {
				bool isEndOfList = false;
				bool targetFound = false;
				try {
					rulePtr = &*this->grublistCfg->proxies.getNextVisibleRule(rulePtr, -direction);
				} catch (ProxyList::Exception e) {
					isEndOfList = true;
					rulePtr = this->grublistCfg->proxies.getProxyByRule(rulePtr)->getParentRule(rulePtr);
				}
				if (!isEndOfList && rulePtr->type == Rule::SUBMENU) {
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
			movedRules.push_back(&*this->grublistCfg->proxies.getNextVisibleRule(static_cast<Rule*>(movedRules.back()), direction));
		}

		this->syncListView_load();
		this->listCfgDlg->selectRules(movedRules);
		this->modificationsUnsaved = true;
	} catch (GrublistCfg::Exception e) {
		if (e == GrublistCfg::NO_MOVE_TARGET_FOUND)
			this->listCfgDlg->showErrorMessage(gettext("cannot move this entry"));
		else
			throw e;
	}
}


void GrubCustomizer::createSubmenu(std::list<void*> childItems) {
	Rule* firstRule = static_cast<Rule*>(childItems.front());
	Rule* newItem = this->grublistCfg->createSubmenu(firstRule);
	this->syncListView_load();
	this->moveRules(childItems, -1);
	this->listCfgDlg->selectRule(newItem, true);
}

void GrubCustomizer::removeSubmenu(std::list<void*> childItems) {
	Rule* firstItem = this->grublistCfg->splitSubmenu(static_cast<Rule*>(childItems.front()));
	std::list<void*> movedRules;
	movedRules.push_back(firstItem);
	for (int i = 1; i < childItems.size(); i++) {
		movedRules.push_back(&*this->grublistCfg->proxies.getNextVisibleRule(static_cast<Rule*>(movedRules.back()), 1));
	}

	this->moveRules(movedRules, -1);
}

void GrubCustomizer::revertList() {
	int remaining = this->grublistCfg->proxies.size();
	while (remaining) {
		this->grublistCfg->proxies.deleteProxy(&this->grublistCfg->proxies.front());
		assert(this->grublistCfg->proxies.size() < remaining); // make sure that the proxy has really been deleted to prevent an endless loop
		remaining = this->grublistCfg->proxies.size();
	}
	int i = 50; // unknown scripts starting at position 50
	for (std::list<Script>::iterator iter = this->grublistCfg->repository.begin(); iter != this->grublistCfg->repository.end(); iter++) {
		Proxy newProxy(*iter);
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
	this->syncListView_load();
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

	std::string nColor = this->settings->getValue("GRUB_COLOR_NORMAL");
	std::string hColor = this->settings->getValue("GRUB_COLOR_HIGHLIGHT");
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

	std::string wallpaper_key = this->env.useDirectBackgroundProps ? "GRUB_BACKGROUND" : "GRUB_MENU_PICTURE";
	std::string menuPicturePath = this->settings->getValue(wallpaper_key);
	bool menuPicIsInGrubDir = false;
	if (menuPicturePath != "" && menuPicturePath[0] != '/'){
		menuPicturePath = env.output_config_dir + "/" + menuPicturePath;
		menuPicIsInGrubDir = true;
	}

	this->settingsDlg->setFontName(this->settings->grubFont);

	if (this->settings->isActive(wallpaper_key) && menuPicturePath != ""){
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

void GrubCustomizer::updateBackgroundImage(){
	if (!this->env.useDirectBackgroundProps) {
		this->settings->setValue("GRUB_MENU_PICTURE", this->settingsDlg->getBackgroundImagePath());
		this->settings->setIsActive("GRUB_MENU_PICTURE", true);
		this->settings->setIsExport("GRUB_MENU_PICTURE", true);
	} else {
		this->settings->setValue("GRUB_BACKGROUND", this->settingsDlg->getBackgroundImagePath());
		this->settings->setIsActive("GRUB_BACKGROUND", true);
	}
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

void GrubCustomizer::updateFontSettings(bool removeFont) {
	std::string fontName;
	int fontSize = -1;
	if (!removeFont) {
		fontName = this->settingsDlg->getFontName();
		fontSize = this->settingsDlg->getFontSize();;
	}
	this->settings->grubFont = fontName;
	this->settings->grubFontSize = fontSize;
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::removeBackgroundImage(){
	if (!this->env.useDirectBackgroundProps) {
		this->settings->setIsActive("GRUB_MENU_PICTURE", false);
	} else {
		this->settings->setIsActive("GRUB_BACKGROUND", false);
	}
	this->syncSettings();
	this->modificationsUnsaved = true;
}

void GrubCustomizer::hideSettingsDialog(){
	this->settingsDlg->hide();
	if (this->settings->reloadRequired()){
		this->getThreadController().startLoadThread(true);
	}
}

void GrubCustomizer::updateTimeoutSetting(){
	if (this->settingsDlg->getShowMenuCheckboxState()){
		this->settings->setValue("GRUB_TIMEOUT", this->settingsDlg->getTimeoutValueString());
	}
	else {
		this->settings->setValue("GRUB_HIDDEN_TIMEOUT", this->settingsDlg->getTimeoutValueString());
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
