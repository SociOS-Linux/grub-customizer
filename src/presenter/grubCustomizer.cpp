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

GrubCustomizer::GrubCustomizer(Model_Env& env)
	: grublistCfg(NULL), settingsDlg(NULL), settings(NULL),
	  installer(NULL), installDlg(NULL), settingsOnDisk(NULL), entryAddDlg(NULL),
	  savedListCfg(NULL),
	  fbResolutionsGetter(NULL), deviceDataList(NULL),
	  mountTable(NULL), aboutDialog(NULL),
	 env(env),
	 contentParserFactory(NULL), currentContentParser(NULL),
	 grubEnvEditor(NULL), threadController(NULL),
	 entryNameMapper(NULL)
{
}


void GrubCustomizer::setListCfg(Model_ListCfg& grublistCfg){
	this->grublistCfg = &grublistCfg;
}

void GrubCustomizer::setSettingsDialog(View_Settings& settingsDlg){
	this->settingsDlg = &settingsDlg;
}

void GrubCustomizer::setSettingsManager(Model_SettingsManagerData& settings){
	this->settings = &settings;
}

void GrubCustomizer::setSettingsBuffer(Model_SettingsManagerData& settings){
	this->settingsOnDisk = &settings;
}

void GrubCustomizer::setInstaller(Model_Installer& installer){
	this->installer = &installer;
}
void GrubCustomizer::setInstallDlg(View_Installer& installDlg){
	this->installDlg = &installDlg;
}
void GrubCustomizer::setScriptAddDlg(View_Trash& scriptAddDlg){
	this->entryAddDlg = &scriptAddDlg;
}

void GrubCustomizer::setSavedListCfg(Model_ListCfg& savedListCfg){
	this->savedListCfg = &savedListCfg;
}

void GrubCustomizer::setFbResolutionsGetter(Model_FbResolutionsGetter& fbResolutionsGetter){
	this->fbResolutionsGetter = &fbResolutionsGetter;
}

void GrubCustomizer::setDeviceDataList(Model_DeviceDataList& deviceDataList){
	this->deviceDataList = &deviceDataList;
}

void GrubCustomizer::setMountTable(Model_MountTable& mountTable){
	this->mountTable = &mountTable;
}

void GrubCustomizer::setAboutDialog(View_About& aboutDialog){
	this->aboutDialog = &aboutDialog;
}

void GrubCustomizer::setThreadController(ThreadController& threadController) {
	this->threadController = &threadController;
}

void GrubCustomizer::setContentParserFactory(ContentParserFactory& contentParserFactory) {
	this->contentParserFactory = &contentParserFactory;
}

void GrubCustomizer::setGrubEnvEditor(View_EnvEditor& envEditor) {
	this->grubEnvEditor = &envEditor;
}

void GrubCustomizer::setEntryNameMapper(Mapper_EntryName& mapper) {
	this->entryNameMapper = &mapper;
}

ThreadController& GrubCustomizer::getThreadController() {
	if (this->threadController == NULL) {
		throw INCOMPLETE;
	}
	return *this->threadController;
}

Model_FbResolutionsGetter& GrubCustomizer::getFbResolutionsGetter() {
	return *this->fbResolutionsGetter;
}

void GrubCustomizer::updateSettingsDlg(){
	std::list<Model_Proxylist_Item> entryTitles = this->grublistCfg->proxies.generateEntryTitleList();
	std::list<std::string> labelListToplevel  = this->grublistCfg->proxies.getToplevelEntryTitles();

	this->settingsDlg->clearDefaultEntryChooser();
	for (std::list<Model_Proxylist_Item>::iterator iter = entryTitles.begin(); iter != entryTitles.end(); iter++) {
		this->settingsDlg->addEntryToDefaultEntryChooser(iter->labelPathValue, iter->labelPathLabel, iter->numericPathValue, iter->numericPathLabel);
	}

	this->settingsDlg->setPreviewEntryTitles(labelListToplevel);

	this->syncSettings();
}

void GrubCustomizer::showEnvEditor(bool resetPartitionChooser) {
	this->grubEnvEditor->setEnvSettings(this->env.getProperties(), this->env.getRequiredProperties(), this->env.getValidProperties());
	this->grubEnvEditor->show(resetPartitionChooser);
}

void GrubCustomizer::showSettingsDlg(){
	this->settingsDlg->show(env.burgMode);
}

void GrubCustomizer::save_thread(){
	this->log("writing settings file", Logger::IMPORTANT_EVENT);
	this->settings->save();
	if (this->settings->color_helper_required) {
		this->grublistCfg->addColorHelper();
	}
	this->log("writing grub list configuration", Logger::IMPORTANT_EVENT);
	this->grublistCfg->save();
	this->env.activeThreadCount--;
}


void GrubCustomizer::showAboutDialog(){
	this->aboutDialog->show();
}

void GrubCustomizer::generateSubmountpointSelection(std::string const& prefix){
	this->grubEnvEditor->removeAllSubmountpoints();

	//create new submountpoint checkbuttons
	for (Model_MountTable::const_iterator iter = mountTable->begin(); iter != mountTable->end(); iter++){
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
			this->env.init(env.burgMode ? Model_Env::BURG_MODE : Model_Env::GRUB_MODE, PARTCHOOSER_MOUNTPOINT);
			this->generateSubmountpointSelection(PARTCHOOSER_MOUNTPOINT);
			this->showEnvEditor();
		}
		catch (Model_MountTable_Mountpoint::Exception const& e) {
			if (e == Model_MountTable_Mountpoint::MOUNT_FAILED){
				this->grubEnvEditor->showErrorMessage(View_EnvEditor::MOUNT_FAILED);
				this->switchPartition("");
			}
		}
		catch (Model_MountTable::Exception const& e) {
			if (e == Model_MountTable::MOUNT_ERR_NO_FSTAB){
				this->grubEnvEditor->showErrorMessage(View_EnvEditor::MOUNT_ERR_NO_FSTAB);
				mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT).umount();
				this->switchPartition("");
			}
		}
	} else {
		this->env.init(env.burgMode ? Model_Env::BURG_MODE : Model_Env::GRUB_MODE, selectedDevice);
		this->showEnvEditor(true);
	}
}

void GrubCustomizer::switchBootloaderType(int newTypeIndex) {
	this->env.init(newTypeIndex == 0 ? Model_Env::GRUB_MODE : Model_Env::BURG_MODE, this->env.cfg_dir_prefix);
	this->showEnvEditor();
}

void GrubCustomizer::updateGrubEnvOptions() {
	this->env.setProperties(this->grubEnvEditor->getEnvSettings());
	this->showEnvEditor();
}

void GrubCustomizer::applyEnvEditor(bool saveConfig){
//	listCfgDlg->setLockState(1|2|8);
//	this->syncSettings();
	settingsDlg->hide();
	entryAddDlg->hide();
	bool isBurgMode = this->grubEnvEditor->getBootloaderType() == 1;
	grubEnvEditor->hide();

	if (saveConfig) {
		this->env.save();
	}
	this->getAllControllers().mainController->reInitAction(isBurgMode);
}


void GrubCustomizer::mountSubmountpoint(std::string const& submountpoint){
	try {
		this->mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT + submountpoint).mount();
	}
	catch (Model_MountTable_Mountpoint::Exception const& e){
		if (e == Model_MountTable_Mountpoint::MOUNT_FAILED){
			this->grubEnvEditor->showErrorMessage(View_EnvEditor::SUB_MOUNT_FAILED);
		}
		this->grubEnvEditor->setSubmountpointSelectionState(submountpoint, false);
		this->grubEnvEditor->show();
	}
}

void GrubCustomizer::umountSubmountpoint(std::string const& submountpoint){
	try {
		this->mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT + submountpoint).umount();
	}
	catch (Model_MountTable_Mountpoint::Exception const& e){
		if (e == Model_MountTable_Mountpoint::UMOUNT_FAILED){
			this->grubEnvEditor->showErrorMessage(View_EnvEditor::SUB_UMOUNT_FAILED);
		}
		this->grubEnvEditor->setSubmountpointSelectionState(submountpoint, true);
		this->grubEnvEditor->show();
	}
}

void GrubCustomizer::showInstallDialog(){
	installDlg->show();
}

void GrubCustomizer::installGrub(std::string device){
	this->env.activeThreadCount++;
	installer->threadable_install(device);
	this->env.activeThreadCount--;
	if (this->env.activeThreadCount == 0 && this->env.quit_requested) {
		this->getAllControllers().mainController->exitAction(true);
	}
}

void GrubCustomizer::showMessageGrubInstallCompleted(std::string const& msg){
	installDlg->showMessageGrubInstallCompleted(msg);
}

void GrubCustomizer::showEntryAddDlg(){
	entryAddDlg->clear();

	std::list<Model_Entry*> removedEntries = this->grublistCfg->getRemovedEntries();
	for (std::list<Model_Entry*>::iterator iter = removedEntries.begin(); iter != removedEntries.end(); iter++) {
		Model_Script* script = this->grublistCfg->repository.getScriptByEntry(**iter);

		std::string name = (*iter)->name;
		name = this->entryNameMapper->map(&**iter, name, script->name);

		entryAddDlg->addItem(name, (*iter)->type != Model_Entry::MENUENTRY, script->name, *iter);
	}

	entryAddDlg->show();
}

void GrubCustomizer::showEntryEditDlg(void* rule) {
	this->getAllControllers().entryEditController->showAction(rule);
}

void GrubCustomizer::showEntryCreateDlg() {
	this->getAllControllers().entryEditController->showCreatorAction();
}

void GrubCustomizer::addEntryFromEntryAddDlg(){
	std::list<void*> entries = entryAddDlg->getSelectedEntries();
	this->getAllControllers().mainController->addEntriesAction(entries);
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
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->settings->begin(); iter != this->settings->end(); this->settings->iter_to_next_setting(iter)){
		this->settingsDlg->addCustomOption(iter->isActive, iter->name, iter->value);
	}
	this->settingsDlg->selectCustomOption(sel);
	std::string defEntry = this->settings->getValue("GRUB_DEFAULT");
	if (defEntry == "saved"){
		this->settingsDlg->setActiveDefEntryOption(View_Settings::DEF_ENTRY_SAVED);
	}
	else {
		this->settingsDlg->setActiveDefEntryOption(View_Settings::DEF_ENTRY_PREDEFINED);
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
		this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_FONT).selectColor(nColor.substr(0, nColor.find('/')));
		this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor(nColor.substr(nColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_FONT).selectColor("light-gray");
		this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor("black");
	}
	if (hColor != ""){
		this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor(hColor.substr(0, hColor.find('/')));
		this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor(hColor.substr(hColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor("magenta");
		this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor("black");
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

	if (this->settings->reloadRequired()) {
		this->getAllControllers().mainController->showReloadRecommendationAction();
	}
}

void GrubCustomizer::updateDefaultSetting(){
	if (this->settingsDlg->getActiveDefEntryOption() == View_Settings::DEF_ENTRY_SAVED){
		this->settings->setValue("GRUB_DEFAULT", "saved");
		this->settings->setValue("GRUB_SAVEDEFAULT", "true");
		this->settings->setIsActive("GRUB_SAVEDEFAULT", true);
	}
	else {
		this->settings->setValue("GRUB_DEFAULT", this->settingsDlg->getSelectedDefaultGrubValue());
		this->settings->setValue("GRUB_SAVEDEFAULT", "false");
	}
	this->syncSettings();
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::updateCustomSetting(std::string const& name){
	View_Settings::CustomOption c = this->settingsDlg->getCustomOption(name);
	this->settings->renameItem(c.old_name, c.name);
	this->settings->setValue(c.name, c.value);
	this->settings->setIsActive(c.name, c.isActive);
	this->syncSettings();
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::addNewCustomSettingRow(){
	std::string newSettingName = this->settings->addNewItem();
	this->syncSettings();
}
void GrubCustomizer::removeCustomSettingRow(std::string const& name){
	this->settings->removeItem(name);
	this->syncSettings();
	this->env.modificationsUnsaved = true;
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
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::updateOsProberSetting(){
	this->settings->setValue("GRUB_DISABLE_OS_PROBER", this->settingsDlg->getOsProberCheckboxState() ? "false" : "true");
	this->settings->setIsActive("GRUB_DISABLE_OS_PROBER", !this->settingsDlg->getOsProberCheckboxState());
	this->syncSettings();
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::updateKernalParams(){
	this->settings->setValue("GRUB_CMDLINE_LINUX_DEFAULT", this->settingsDlg->getKernelParams());
	this->syncSettings();
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::updateUseCustomResolution(){
	if (this->settings->getValue("GRUB_GFXMODE") == "") {
		this->settings->setValue("GRUB_GFXMODE", "saved"); //use saved as default value (if empty)
	}
	this->settings->setIsActive("GRUB_GFXMODE", this->settingsDlg->getResolutionCheckboxState());
	this->syncSettings();
	this->env.modificationsUnsaved = true;
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
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::updateColorSettings(){
	if (this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() != "" && this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor() != ""){
		this->settings->setValue("GRUB_COLOR_NORMAL", this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() + "/" + this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor());
		this->settings->setIsActive("GRUB_COLOR_NORMAL", true);
		this->settings->setIsExport("GRUB_COLOR_NORMAL", true);
	}
	if (this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() != "" && this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor() != ""){
		this->settings->setValue("GRUB_COLOR_HIGHLIGHT", this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() + "/" + this->settingsDlg->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor());
		this->settings->setIsActive("GRUB_COLOR_HIGHLIGHT", true);
		this->settings->setIsExport("GRUB_COLOR_HIGHLIGHT", true);
	}
	this->syncSettings();
	this->env.modificationsUnsaved = true;
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
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::removeBackgroundImage(){
	if (!this->env.useDirectBackgroundProps) {
		this->settings->setIsActive("GRUB_MENU_PICTURE", false);
	} else {
		this->settings->setIsActive("GRUB_BACKGROUND", false);
	}
	this->syncSettings();
	this->env.modificationsUnsaved = true;
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
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::updateCustomResolution(){
	this->settings->setValue("GRUB_GFXMODE", this->settingsDlg->getResolution());
	this->syncSettings();
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::updateGenerateRecoverySetting(){
	if (this->settings->getValue("GRUB_DISABLE_LINUX_RECOVERY") != "true") {
		this->settings->setValue("GRUB_DISABLE_LINUX_RECOVERY", "true");
	}
	this->settings->setIsActive("GRUB_DISABLE_LINUX_RECOVERY", !this->settingsDlg->getRecoveryCheckboxState());
	this->syncSettings();
	this->env.modificationsUnsaved = true;
}

void GrubCustomizer::grubEnvSetRootDeviceName(std::string const& rootDevice) {
	this->grubEnvEditor->setRootDeviceName(rootDevice);
}

void GrubCustomizer::grubEnvsetEnvSettings(std::map<std::string, std::string> const& props, std::list<std::string> const& requiredProps, std::list<std::string> const& validProps) {
	this->grubEnvEditor->setEnvSettings(props, requiredProps, validProps);
}

void GrubCustomizer::grubEnvShow(bool resetPartitionChooser = false) {
	this->grubEnvEditor->show();
}
