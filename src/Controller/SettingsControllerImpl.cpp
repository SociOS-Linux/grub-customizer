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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "SettingsControllerImpl.h"

SettingsControllerImpl::SettingsControllerImpl(Model_Env& env)
	: ControllerAbstract("settings"),
	  grublistCfg(NULL), view(NULL), settings(NULL),
	  settingsOnDisk(NULL),
	  fbResolutionsGetter(NULL),
	 env(env),
	 threadController(NULL)
{
}


void SettingsControllerImpl::setListCfg(Model_ListCfg& grublistCfg){
	this->grublistCfg = &grublistCfg;
}

void SettingsControllerImpl::setView(View_Settings& settingsDlg){
	this->view = &settingsDlg;
}

void SettingsControllerImpl::setSettingsManager(Model_SettingsManagerData& settings){
	this->settings = &settings;
}

void SettingsControllerImpl::setSettingsBuffer(Model_SettingsManagerData& settings){
	this->settingsOnDisk = &settings;
}

void SettingsControllerImpl::setFbResolutionsGetter(Model_FbResolutionsGetter& fbResolutionsGetter){
	this->fbResolutionsGetter = &fbResolutionsGetter;
}

void SettingsControllerImpl::setThreadController(ThreadController& threadController) {
	this->threadController = &threadController;
}

ThreadController& SettingsControllerImpl::getThreadController() {
	if (this->threadController == NULL) {
		throw ConfigException("missing ThreadController", __FILE__, __LINE__);
	}
	return *this->threadController;
}

Model_FbResolutionsGetter& SettingsControllerImpl::getFbResolutionsGetter() {
	return *this->fbResolutionsGetter;
}

void SettingsControllerImpl::loadResolutionsAction() {
	this->logActionBegin("load-resolutions");
	try {
		this->fbResolutionsGetter->load();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateSettingsDataAction(){
	this->logActionBegin("update-settings-data");
	try {
		std::list<Model_Proxylist_Item> entryTitles = this->grublistCfg->proxies.generateEntryTitleList();
		std::list<std::string> labelListToplevel  = this->grublistCfg->proxies.getToplevelEntryTitles();

		this->view->clearDefaultEntryChooser();
		for (std::list<Model_Proxylist_Item>::iterator iter = entryTitles.begin(); iter != entryTitles.end(); iter++) {
			this->view->addEntryToDefaultEntryChooser(iter->labelPathValue, iter->labelPathLabel, iter->numericPathValue, iter->numericPathLabel);
		}

		this->view->setPreviewEntryTitles(labelListToplevel);

		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::showSettingsDlg(){
	this->view->show(env.burgMode);
}

void SettingsControllerImpl::updateResolutionlistAction(){
	this->logActionBegin("update-resolutionlist");
	try {
		const std::list<std::string>& data = this->fbResolutionsGetter->getData();
		this->view->clearResolutionChooser();
		for (std::list<std::string>::const_iterator iter = data.begin(); iter != data.end(); iter++) {
			this->view->addResolution(*iter);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateResolutionlistThreadedAction() {
	this->logActionBeginThreaded("update-resolutionlist-threaded");
	try {
		this->threadController->updateSettingsDlgResolutionList();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorThreadedAction(e);
	}
	this->logActionEndThreaded();
}

void SettingsControllerImpl::syncSettings(){
	std::string sel = this->view->getSelectedCustomOption();
	this->view->removeAllSettingRows();
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->settings->begin(); iter != this->settings->end(); this->settings->iter_to_next_setting(iter)){
		this->view->addCustomOption(iter->isActive, iter->name, iter->value);
	}
	this->view->selectCustomOption(sel);
	std::string defEntry = this->settings->getValue("GRUB_DEFAULT");
	if (defEntry == "saved"){
		this->view->setActiveDefEntryOption(View_Settings::DEF_ENTRY_SAVED);
	}
	else {
		this->view->setActiveDefEntryOption(View_Settings::DEF_ENTRY_PREDEFINED);
		this->view->setDefEntry(defEntry);
	}

	this->view->setShowMenuCheckboxState(!this->settings->isActive("GRUB_HIDDEN_TIMEOUT", true));
	this->view->setOsProberCheckboxState(!this->settings->isActive("GRUB_DISABLE_OS_PROBER", true));

	std::string timeoutStr;
	if (this->view->getShowMenuCheckboxState())
		timeoutStr = this->settings->getValue("GRUB_TIMEOUT");
	else
		timeoutStr = this->settings->getValue("GRUB_HIDDEN_TIMEOUT");

	if (timeoutStr == "" || timeoutStr.find_first_not_of("0123456789") != -1) {
		timeoutStr = "10"; //default value
	}
	std::istringstream in(timeoutStr);
	int timeout;
	in >> timeout;
	this->view->setTimeoutValue(timeout);

	this->view->setKernelParams(this->settings->getValue("GRUB_CMDLINE_LINUX_DEFAULT"));
	this->view->setRecoveryCheckboxState(!this->settings->isActive("GRUB_DISABLE_LINUX_RECOVERY", true));

	this->view->setResolutionCheckboxState(this->settings->isActive("GRUB_GFXMODE", true));
	this->view->setResolution(this->settings->getValue("GRUB_GFXMODE"));

	std::string nColor = this->settings->getValue("GRUB_COLOR_NORMAL");
	std::string hColor = this->settings->getValue("GRUB_COLOR_HIGHLIGHT");
	if (nColor != ""){
		this->view->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_FONT).selectColor(nColor.substr(0, nColor.find('/')));
		this->view->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor(nColor.substr(nColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->view->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_FONT).selectColor("light-gray");
		this->view->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor("black");
	}
	if (hColor != ""){
		this->view->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor(hColor.substr(0, hColor.find('/')));
		this->view->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor(hColor.substr(hColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->view->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor("magenta");
		this->view->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor("black");
	}

	std::string wallpaper_key = this->env.useDirectBackgroundProps ? "GRUB_BACKGROUND" : "GRUB_MENU_PICTURE";
	std::string menuPicturePath = this->settings->getValue(wallpaper_key);
	bool menuPicIsInGrubDir = false;
	if (menuPicturePath != "" && menuPicturePath[0] != '/'){
		menuPicturePath = env.output_config_dir + "/" + menuPicturePath;
		menuPicIsInGrubDir = true;
	}

	this->view->setFontName(this->settings->grubFont);

	if (this->settings->isActive(wallpaper_key) && menuPicturePath != ""){
		this->view->setBackgroundImagePreviewPath(menuPicturePath, menuPicIsInGrubDir);
	}
	else {
		this->view->setBackgroundImagePreviewPath("", menuPicIsInGrubDir);
	}

	if (this->settings->reloadRequired()) {
		this->getAllControllers().mainController->showReloadRecommendationAction();
	}
}

void SettingsControllerImpl::updateDefaultSystemAction(){
	this->logActionBegin("update-default-system");
	try {
		if (this->view->getActiveDefEntryOption() == View_Settings::DEF_ENTRY_SAVED){
			this->settings->setValue("GRUB_DEFAULT", "saved");
			this->settings->setValue("GRUB_SAVEDEFAULT", "true");
			this->settings->setIsActive("GRUB_SAVEDEFAULT", true);
		}
		else {
			this->settings->setValue("GRUB_DEFAULT", this->view->getSelectedDefaultGrubValue());
			this->settings->setValue("GRUB_SAVEDEFAULT", "false");
		}
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateCustomSettingAction(std::string const& name){
	this->logActionBegin("update-custom-setting");
	try {
		View_Settings::CustomOption c = this->view->getCustomOption(name);
		this->settings->renameItem(c.old_name, c.name);
		this->settings->setValue(c.name, c.value);
		this->settings->setIsActive(c.name, c.isActive);
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::addCustomSettingAction(){
	this->logActionBegin("add-custom-setting");
	try {
		std::string newSettingName = this->settings->addNewItem();
		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}
void SettingsControllerImpl::removeCustomSettingAction(std::string const& name){
	this->logActionBegin("remove-custom-setting");
	try {
		this->settings->removeItem(name);
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateShowMenuSettingAction(){
	this->logActionBegin("update-show-menu-setting");
	try {
		std::string val = this->settings->getValue("GRUB_HIDDEN_TIMEOUT");
		if (val == "" || val.find_first_not_of("0123456789") != -1) {
			this->settings->setValue("GRUB_HIDDEN_TIMEOUT", "0"); //create this entry - if it has an invalid value
		}
		this->settings->setIsActive("GRUB_HIDDEN_TIMEOUT", !this->view->getShowMenuCheckboxState());
		if (!this->view->getShowMenuCheckboxState() && this->view->getOsProberCheckboxState()){
			this->view->showHiddenMenuOsProberConflictMessage();
		}
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateOsProberSettingAction(){
	this->logActionBegin("update-os-prober-setting");
	try {
		this->settings->setValue("GRUB_DISABLE_OS_PROBER", this->view->getOsProberCheckboxState() ? "false" : "true");
		this->settings->setIsActive("GRUB_DISABLE_OS_PROBER", !this->view->getOsProberCheckboxState());
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateKernelParamsAction(){
	this->logActionBegin("update-kernel-params");
	try {
		this->settings->setValue("GRUB_CMDLINE_LINUX_DEFAULT", this->view->getKernelParams());
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateUseCustomResolutionAction(){
	this->logActionBegin("update-use-custom-resolution");
	try {
		if (this->settings->getValue("GRUB_GFXMODE") == "") {
			this->settings->setValue("GRUB_GFXMODE", "saved"); //use saved as default value (if empty)
		}
		this->settings->setIsActive("GRUB_GFXMODE", this->view->getResolutionCheckboxState());
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateBackgroundImageAction(){
	this->logActionBegin("update-background-image");
	try {
		if (!this->env.useDirectBackgroundProps) {
			this->settings->setValue("GRUB_MENU_PICTURE", this->view->getBackgroundImagePath());
			this->settings->setIsActive("GRUB_MENU_PICTURE", true);
			this->settings->setIsExport("GRUB_MENU_PICTURE", true);
		} else {
			this->settings->setValue("GRUB_BACKGROUND", this->view->getBackgroundImagePath());
			this->settings->setIsActive("GRUB_BACKGROUND", true);
		}
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateColorSettingsAction(){
	this->logActionBegin("update-color-settings");
	try {
		if (this->view->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() != "" && this->view->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor() != ""){
			this->settings->setValue("GRUB_COLOR_NORMAL", this->view->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() + "/" + this->view->getColorChooser(View_Settings::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor());
			this->settings->setIsActive("GRUB_COLOR_NORMAL", true);
			this->settings->setIsExport("GRUB_COLOR_NORMAL", true);
		}
		if (this->view->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() != "" && this->view->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor() != ""){
			this->settings->setValue("GRUB_COLOR_HIGHLIGHT", this->view->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() + "/" + this->view->getColorChooser(View_Settings::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor());
			this->settings->setIsActive("GRUB_COLOR_HIGHLIGHT", true);
			this->settings->setIsExport("GRUB_COLOR_HIGHLIGHT", true);
		}
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateFontSettingsAction(bool removeFont) {
	this->logActionBegin("update-font-settings");
	try {
		std::string fontName;
		int fontSize = -1;
		if (!removeFont) {
			fontName = this->view->getFontName();
			fontSize = this->view->getFontSize();;
		}
		this->settings->grubFont = fontName;
		this->settings->grubFontSize = fontSize;
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::removeBackgroundImageAction(){
	this->logActionBegin("remove-background-image");
	try {
		if (!this->env.useDirectBackgroundProps) {
			this->settings->setIsActive("GRUB_MENU_PICTURE", false);
		} else {
			this->settings->setIsActive("GRUB_BACKGROUND", false);
		}
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::hideAction(){
	this->logActionBegin("hide");
	try {
		this->view->hide();
		if (this->settings->reloadRequired()){
			this->getThreadController().startLoadThread(true);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::showAction(bool burgMode) {
	this->logActionBegin("show");
	try {
		this->view->show(burgMode);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateTimeoutSettingAction(){
	this->logActionBegin("update-timeout-setting");
	try {
		if (this->view->getShowMenuCheckboxState()){
			this->settings->setValue("GRUB_TIMEOUT", this->view->getTimeoutValueString());
		}
		else {
			this->settings->setValue("GRUB_HIDDEN_TIMEOUT", this->view->getTimeoutValueString());
		}
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateCustomResolutionAction(){
	this->logActionBegin("update-custom-resolution");
	try {
		this->settings->setValue("GRUB_GFXMODE", this->view->getResolution());
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::updateRecoverySettingAction(){
	this->logActionBegin("update-recovery-setting");
	try {
		if (this->settings->getValue("GRUB_DISABLE_LINUX_RECOVERY") != "true") {
			this->settings->setValue("GRUB_DISABLE_LINUX_RECOVERY", "true");
		}
		this->settings->setIsActive("GRUB_DISABLE_LINUX_RECOVERY", !this->view->getRecoveryCheckboxState());
		this->syncSettings();
		this->env.modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void SettingsControllerImpl::syncAction() {
	this->logActionBegin("sync");
	try {
		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}
