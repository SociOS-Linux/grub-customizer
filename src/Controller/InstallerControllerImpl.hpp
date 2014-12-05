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

#ifndef INSTALLERCONTROLLERIMPL_H_
#define INSTALLERCONTROLLERIMPL_H_

#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.hpp"

#include "../Model/Env.hpp"

#include "../Model/Installer.hpp"
#include "../View/Installer.hpp"
#include "../View/Trait/ViewAware.hpp"

#include "../Controller/ControllerAbstract.hpp"
#include "../Controller/Trait/ThreadControllerAware.hpp"

#include "../lib/Exception.hpp"

#include "InstallerController.hpp"
#include "ThreadController.hpp"


class InstallerControllerImpl :
	public ControllerAbstract,
	public InstallerController,
	public View_Trait_ViewAware<View_Installer>,
	public Trait_ThreadControllerAware,
	public Model_Installer_Connection,
	public Model_Env_Connection
{
public:
	ThreadController& getThreadController() {
		if (this->threadController == NULL) {
			throw ConfigException("missing ThreadController", __FILE__, __LINE__);
		}
		return *this->threadController;
	}


	InstallerControllerImpl() : ControllerAbstract("installer")
	{
	}

	
	void showAction() {
		this->logActionBegin("show");
		try {
			view->show();
		} catch (Exception const& e) {
			this->getAllControllers().errorController->errorAction(e);
		}
		this->logActionEnd();
	}

	void installGrubAction(std::string device) {
		this->logActionBegin("install-grub");
		try {
			this->getThreadController().startGrubInstallThread(device);
		} catch (Exception const& e) {
			this->getAllControllers().errorController->errorAction(e);
		}
		this->logActionEnd();
	}

	void installGrubThreadedAction(std::string device) {
		this->logActionBeginThreaded("install-grub-threaded");
		try {
			this->env->activeThreadCount++;
			installer->threadable_install(device);
			this->env->activeThreadCount--;
			if (this->env->activeThreadCount == 0 && this->env->quit_requested) {
				this->getAllControllers().mainController->exitAction(true);
			}
		} catch (Exception const& e) {
			this->getAllControllers().errorController->errorThreadedAction(e);
		}
		this->logActionEndThreaded();
	}

	void showMessageAction(std::string const& msg) {
		this->logActionBegin("show-message");
		try {
			view->showMessageGrubInstallCompleted(msg);
		} catch (Exception const& e) {
			this->getAllControllers().errorController->errorAction(e);
		}
		this->logActionEnd();
	}

};

#endif
