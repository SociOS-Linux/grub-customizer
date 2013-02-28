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
	: env(env), view(NULL), ControllerAbstract("theme"), themeManager(NULL)
{
}

void ThemeControllerImpl::setView(View_Theme& view) {
	this->view = &view;
}

void ThemeControllerImpl::setThemeManager(Model_ThemeManager& themeManager) {
	this->themeManager = &themeManager;
}

void ThemeControllerImpl::loadThemesAction() {
	this->logActionBegin("load-themes");
	try {
		this->themeManager->load();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::loadThemeAction(std::string const& name) {
	this->logActionBegin("load-theme");
	try {
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
