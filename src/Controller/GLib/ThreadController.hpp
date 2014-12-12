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
	Rule* _cachedRulePtr;
	std::string _cachedThemeFileName;
public:
	GLib_ThreadController() : _cachedRulePtr(nullptr) {}

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

	void startEdit(Rule* rule) {
		this->_cachedRulePtr = rule;
	
		Glib::signal_timeout().connect_once(sigc::mem_fun(this, &GLib_ThreadController::_execRuleEdit), 10);
	}

	void startThemeFileEdit(std::string const& fileName) {
		this->_cachedThemeFileName = fileName;
		Glib::signal_timeout().connect_once(sigc::mem_fun(this, &GLib_ThreadController::_execThemeFileEdit), 10);
	}

private:
	void _execLoad(bool preserveConfig) {
		this->getAllControllers().mainController->loadThreadedAction(preserveConfig);
	}

	void _execSave() {
		this->getAllControllers().mainController->saveThreadedAction();
	}

	void _execFbResolutionsGetter() {
		this->getAllControllers().settingsController->loadResolutionsAction();
	}

	void _execInstallGrub(std::string const& device) {
		this->getAllControllers().installerController->installGrubThreadedAction(device);
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
};

#endif
