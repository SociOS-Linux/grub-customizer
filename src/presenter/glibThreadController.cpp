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

#include "glibThreadController.h"

GlibThreadController::GlibThreadController(ControllerCollection& controllers)
	: _controllers(controllers)
{
	disp_sync_load.connect(sigc::mem_fun(this, &GlibThreadController::_execLoadSync));
	disp_sync_save.connect(sigc::mem_fun(this, &GlibThreadController::_execSaveSync));
	disp_thread_died.connect(sigc::mem_fun(this, &GlibThreadController::_execDie));
	disp_settings_loaded.connect(sigc::mem_fun(this, &GlibThreadController::_execActivateSettings));
	disp_updateSettingsDlgResolutionList.connect(sigc::mem_fun(this, &GlibThreadController::_execResolutionListUpdate));
}

void GlibThreadController::syncEntryList(){
	this->disp_sync_load();
}

void GlibThreadController::updateSaveProgress(){
	this->disp_sync_save();
}

void GlibThreadController::updateSettingsDlgResolutionList(){
	this->disp_updateSettingsDlgResolutionList();
}

void GlibThreadController::showThreadDiedError() {
	this->disp_thread_died();
}

void GlibThreadController::enableSettings() {
	this->disp_settings_loaded();
}

void GlibThreadController::startLoadThread(bool preserveConfig) {
	Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &GlibThreadController::_execLoad), preserveConfig), false);
}

void GlibThreadController::startSaveThread() {
	Glib::Thread::create(sigc::mem_fun(this, &GlibThreadController::_execSave), false);
}

void GlibThreadController::startFramebufferResolutionLoader() {
	Glib::Thread::create(sigc::mem_fun(this, &GlibThreadController::_execFbResolutionsGetter), false);
}

void GlibThreadController::startGrubInstallThread(std::string const& device) {
	Glib::Thread::create(sigc::bind<std::string>(sigc::mem_fun(this, &GlibThreadController::_execInstallGrub), device), false);
}

void GlibThreadController::stopApplication() {
	Gtk::Main::quit();
}

void GlibThreadController::_execLoadSync() {
	this->_controllers.mainController->syncLoadStateAction();
}

void GlibThreadController::_execSaveSync() {
	this->_controllers.mainController->syncSaveStateAction();
}

void GlibThreadController::_execLoad(bool preserveConfig) {
	this->_controllers.mainController->loadThreadedAction(preserveConfig);
}

void GlibThreadController::_execSave() {
	this->_controllers.mainController->saveThreadedAction();
}

void GlibThreadController::_execDie() {
	this->_controllers.mainController->dieAction();
}

void GlibThreadController::_execActivateSettings() {
	this->_controllers.mainController->activateSettingsAction();
}

void GlibThreadController::_execResolutionListUpdate() {
	this->_controllers.settingsController->updateResolutionlistAction();
}

void GlibThreadController::_execFbResolutionsGetter() {
	this->_controllers.settingsController->getFbResolutionsGetter().load();
}

void GlibThreadController::_execInstallGrub(std::string const& device) {
	this->_controllers.installerController->installGrubThreadedAction(device);
}
