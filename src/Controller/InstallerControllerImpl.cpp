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

#include "InstallerControllerImpl.h"

InstallerControllerImpl::InstallerControllerImpl(Model_Env& env)
	: installer(NULL), view(NULL),
	  env(env),
	 threadController(NULL)
{
}


void InstallerControllerImpl::setInstaller(Model_Installer& installer){
	this->installer = &installer;
}
void InstallerControllerImpl::setView(View_Installer& installDlg){
	this->view = &installDlg;
}

void InstallerControllerImpl::setThreadController(ThreadController& threadController) {
	this->threadController = &threadController;
}

ThreadController& InstallerControllerImpl::getThreadController() {
	if (this->threadController == NULL) {
		throw ConfigException("missing ThreadController", __FILE__, __LINE__);
	}
	return *this->threadController;
}

void InstallerControllerImpl::showAction(){
	try {
		view->show();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
}

void InstallerControllerImpl::installGrubAction(std::string device){
	try {
		this->getThreadController().startGrubInstallThread(device);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
}

void InstallerControllerImpl::installGrubThreadedAction(std::string device) {
	try {
		this->env.activeThreadCount++;
		installer->threadable_install(device);
		this->env.activeThreadCount--;
		if (this->env.activeThreadCount == 0 && this->env.quit_requested) {
			this->getAllControllers().mainController->exitAction(true);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorThreadedAction(e);
	}
}

void InstallerControllerImpl::showMessageAction(std::string const& msg){
	try {
		view->showMessageGrubInstallCompleted(msg);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
}

