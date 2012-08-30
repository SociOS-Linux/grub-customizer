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
#include "../config.h"

#include "../Model/Env.h"

#include "../Model/Installer.h"
#include "../View/Installer.h"

#include "ThreadController.h"

#include "../Controller/ControllerAbstract.h"

#include "../lib/Exception.h"

#include "InstallerController.h"


class InstallerControllerImpl : public ControllerAbstract, public InstallerController {
	Model_Env& env;
	Model_Installer* installer;
	View_Installer* view;
	ThreadController* threadController;

public:
	void setInstaller(Model_Installer& installer);
	void setView(View_Installer& installDlg);
	void setThreadController(ThreadController& threadController);

	ThreadController& getThreadController();

	InstallerControllerImpl(Model_Env& env);
	
	void showAction();
	void installGrubAction(std::string device);
	void installGrubThreadedAction(std::string device);
	void showMessageAction(std::string const& msg);
};

#endif
