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

#ifndef GLIBTHREADCONTROLLER_H_INCLUDED
#define GLIBTHREADCONTROLLER_H_INCLUDED
#include "../ThreadController.hpp"

#include <glibmm/thread.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/main.h>
#include "../../lib/Type.hpp"
#include "../ControllerCollection.hpp"

class GLib_ThreadController : public ThreadController, public ControllerCollection_Connection {
	Glib::Dispatcher disp_sync_load, disp_sync_save, disp_thread_died, disp_updateSettingsDlgResolutionList, disp_settings_loaded, disp_exception, disp_postSaveActions, disp_config_saving_error;

	Exception _cachedException;
	Rule* _cachedRulePtr;
	std::string _cachedThemeFileName;
	std::string _cachedConfigSavingError;
public:
	GLib_ThreadController() : _cachedException("")
	{
		disp_sync_load.connect(sigc::mem_fun(this, &GLib_ThreadController::_execLoadSync));
		disp_sync_save.connect(sigc::mem_fun(this, &GLib_ThreadController::_execSaveSync));
		disp_thread_died.connect(sigc::mem_fun(this, &GLib_ThreadController::_execDie));
		disp_settings_loaded.connect(sigc::mem_fun(this, &GLib_ThreadController::_execActivateSettings));
		disp_updateSettingsDlgResolutionList.connect(sigc::mem_fun(this, &GLib_ThreadController::_execResolutionListUpdate));
		disp_exception.connect(sigc::mem_fun(this, &GLib_ThreadController::_execShowException));
		disp_postSaveActions.connect(sigc::mem_fun(this, &GLib_ThreadController::_execPostSaveActions));
		disp_config_saving_error.connect(sigc::mem_fun(this, &GLib_ThreadController::_execShowConfigSavingError));
	}

	void syncEntryList() {
		this->disp_sync_load();
	}

	void updateSaveProgress() {
		this->disp_sync_save();
	}

	void updateSettingsDlgResolutionList() {
		this->disp_updateSettingsDlgResolutionList();
	}

	void showThreadDiedError() {
		this->disp_thread_died();
	}

	void enableSettings() {
		this->disp_settings_loaded();
	}

	void startLoadThread(bool preserveConfig) {
		Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &GLib_ThreadController::_execLoad), preserveConfig), false);
	}

	void startSaveThread() {
		Glib::Thread::create(sigc::mem_fun(this, &GLib_ThreadController::_execSave), false);
	}

	void startFramebufferResolutionLoader() {
		Glib::Thread::create(sigc::mem_fun(this, &GLib_ThreadController::_execFbResolutionsGetter), false);
	}

	void startGrubInstallThread(std::string const& device) {
		Glib::Thread::create(sigc::bind<std::string>(sigc::mem_fun(this, &GLib_ThreadController::_execInstallGrub), device), false);
	}

	void stopApplication() {
		Gtk::Main::quit();
	}

	void showException(Exception const& e) {
		this->_cachedException = e;
		this->disp_exception();
	}

	void showConfigSavingError(std::string const& message) {
		this->_cachedConfigSavingError = message;
		this->disp_config_saving_error();
	}

	void startEdit(Rule* rule) {
		this->_cachedRulePtr = rule;
	
		Glib::signal_timeout().connect_once(sigc::mem_fun(this, &GLib_ThreadController::_execRuleEdit), 10);
	}

	void startThemeFileEdit(std::string const& fileName) {
		this->_cachedThemeFileName = fileName;
		Glib::signal_timeout().connect_once(sigc::mem_fun(this, &GLib_ThreadController::_execThemeFileEdit), 10);
	}

	void doPostSaveActions() {
		this->disp_postSaveActions();
	}

private:
	void _execLoadSync() {
		this->getAllControllers().mainController->syncLoadStateAction();
	}

	void _execSaveSync() {
		this->getAllControllers().mainController->syncSaveStateAction();
	}

	void _execLoad(bool preserveConfig) {
		this->getAllControllers().mainController->loadThreadedAction(preserveConfig);
	}

	void _execSave() {
		this->getAllControllers().mainController->saveThreadedAction();
	}

	void _execDie() {
		this->getAllControllers().mainController->dieAction();
	}

	void _execActivateSettings() {
		this->getAllControllers().mainController->activateSettingsAction();
	}

	void _execResolutionListUpdate() {
		this->getAllControllers().settingsController->updateResolutionlistAction();
	}

	void _execFbResolutionsGetter() {
		this->getAllControllers().settingsController->loadResolutionsAction();
	}

	void _execInstallGrub(std::string const& device) {
		this->getAllControllers().installerController->installGrubThreadedAction(device);
	}

	void _execShowException() {
		this->getAllControllers().errorController->errorAction(this->_cachedException);
	}

	void _execRuleEdit() {
		if (this->_cachedRulePtr != NULL) {
			this->getAllControllers().mainController->selectRuleAction(this->_cachedRulePtr, true);
			this->_cachedRulePtr = NULL;
		}
	}

	void _execThemeFileEdit() {
		this->getAllControllers().themeController->startFileEditAction(this->_cachedThemeFileName);
	}

	void _execPostSaveActions() {
		this->getAllControllers().themeController->postSaveAction();
	}

	void _execShowConfigSavingError() {
		this->getAllControllers().mainController->showConfigSavingErrorAction(this->_cachedConfigSavingError);
	}

};

#endif