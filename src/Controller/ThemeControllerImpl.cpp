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

#include "ThemeControllerImpl.h"

ThemeControllerImpl::ThemeControllerImpl(Model_Env& env)
	: env(env), view(NULL), ControllerAbstract("theme"), themeManager(NULL), settings(NULL), grublistCfg(NULL)
{
}

void ThemeControllerImpl::syncSettings() {
	std::string nColor = this->settings->getValue("GRUB_COLOR_NORMAL");
	std::string hColor = this->settings->getValue("GRUB_COLOR_HIGHLIGHT");
	if (nColor != ""){
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_FONT).selectColor(nColor.substr(0, nColor.find('/')));
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor(nColor.substr(nColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_FONT).selectColor("light-gray");
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor("black");
	}
	if (hColor != ""){
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor(hColor.substr(0, hColor.find('/')));
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor(hColor.substr(hColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor("magenta");
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor("black");
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
}

bool ThemeControllerImpl::isImage(std::string const& fileName) {
	std::list<std::string> imageExtensions;
	imageExtensions.push_back("png");
	imageExtensions.push_back("jpg");
	imageExtensions.push_back("bmp");
	imageExtensions.push_back("gif");
	imageExtensions.push_back("pf2"); // not really, but shouldn't be handled as text

	if (fileName.find_last_of(".") != std::string::npos) {
		int dotPos = fileName.find_last_of(".");
		std::string extension = fileName.substr(dotPos + 1);
		if (std::find(imageExtensions.begin(), imageExtensions.end(), extension) != imageExtensions.end()) {
			return true;
		}
	}
	return false;
}

void ThemeControllerImpl::setView(View_Theme& view) {
	this->view = &view;
}

void ThemeControllerImpl::setThemeManager(Model_ThemeManager& themeManager) {
	this->themeManager = &themeManager;
}

void ThemeControllerImpl::setSettingsManager(Model_SettingsManagerData& settings) {
	this->settings = &settings;
}

void ThemeControllerImpl::setListCfg(Model_ListCfg& grublistCfg) {
	this->grublistCfg = &grublistCfg;
}

void ThemeControllerImpl::loadThemesAction() {
	this->logActionBegin("load-themes");
	try {
		this->themeManager->load();
		this->view->clearThemeSelection();
		for (std::list<Model_Theme>::iterator themeIter = this->themeManager->themes.begin(); themeIter != this->themeManager->themes.end(); themeIter++) {
			this->view->addTheme(themeIter->name);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::loadThemeAction(std::string const& name) {
	this->logActionBegin("load-theme");
	try {
		this->view->setEditorType(View_Theme::EDITORTYPE_THEME);
		this->currentTheme = name;
		this->view->clear();
		Model_Theme* theme = &this->themeManager->getTheme(name);
		for (std::list<Model_ThemeFile>::iterator themeFileIter = theme->files.begin(); themeFileIter != theme->files.end(); themeFileIter++) {
			this->view->addFile(themeFileIter->localFileName);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::addThemeFileAction(const std::string& filePath) {
	this->logActionBegin("add-theme-file");
	try {
		try {
			std::string themeName = this->themeManager->addThemeFile(filePath);
			this->loadThemeAction(themeName);
		} catch (InvalidFileTypeException const& e) {
			this->view->showError(View_Theme::ERROR_INVALID_THEME_PACK_FORMAT);
		}
	} catch (const Exception& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::showThemeInstallerAction() {
	this->logActionBegin("show-theme-installer");
	try {
		this->view->showThemeFileChooser();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::showSimpleThemeConfigAction() {
	this->logActionBegin("show-simple-theme-config");
	try {
		this->view->setEditorType(View_Theme::EDITORTYPE_CUSTOM);
		this->updateColorSettingsAction();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::addFileAction() {
	this->logActionBegin("add-file");
	try {

	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::removeFileAction(std::string const& file) {
	this->logActionBegin("remove-file");
	try {

	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::updateEditAreaAction(std::string const& file) {
	this->logActionBegin("update-edit-area");
	try {
		bool isImage = this->isImage(file);
		Model_Theme* theme = &this->themeManager->getTheme(this->currentTheme);
		if (isImage) {
			this->view->setImage(theme->getFullFileName(file));
		} else {
			std::string content = theme->loadFileContent(file);
			this->view->setText(content);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::renameAction(std::string const& oldName, std::string const& newName) {
	this->logActionBegin("rename");
	try {

	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::loadFileAction(std::string const& externalPath) {
	this->logActionBegin("load-file");
	try {

	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::saveTextAction(std::string const& newText) {
	this->logActionBegin("save-text");
	try {

	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::updateBackgroundImageAction(){
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

void ThemeControllerImpl::updateColorSettingsAction(){
	this->logActionBegin("update-color-settings");
	try {
		if (this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() != "" && this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor() != ""){
			this->settings->setValue("GRUB_COLOR_NORMAL", this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() + "/" + this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor());
			this->settings->setIsActive("GRUB_COLOR_NORMAL", true);
			this->settings->setIsExport("GRUB_COLOR_NORMAL", true);
		}
		if (this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() != "" && this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor() != ""){
			this->settings->setValue("GRUB_COLOR_HIGHLIGHT", this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() + "/" + this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor());
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

void ThemeControllerImpl::updateFontSettingsAction(bool removeFont) {
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

void ThemeControllerImpl::removeBackgroundImageAction(){
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

void ThemeControllerImpl::updateSettingsDataAction() {
	this->logActionBegin("update-settings-data");
	try {
		std::list<std::string> labelListToplevel  = this->grublistCfg->proxies.getToplevelEntryTitles();

		this->view->setPreviewEntryTitles(labelListToplevel);

		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::syncAction() {
	this->logActionBegin("sync");
	try {
		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

