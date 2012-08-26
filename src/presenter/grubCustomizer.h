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

#ifndef GTK_CLIENT_INCLUDED
#define GTK_CLIENT_INCLUDED

#include "../Model/ListCfg.h"
#include "../View/Main.h"
#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.h"

#include "../Model/Env.h"

#include "../Model/MountTable.h"

#include "../Model/Installer.h"
#include "../View/Installer.h"

#include "../View/Trash.h"

#include "../View/About.h"
#include "../interface/threadController.h"
#include "../View/EnvEditor.h"
#include "../Mapper/EntryName.h"

#include "../Controller/ControllerAbstract.h"

#include "commonClass.h"

#include "grubCustomizerIface.h"

/**
 * master class of Grub Customizer.
 * Coordinates all the windows (views) and data objects.
 *
 * This application is based on one presenter (this object) and multiple views and models which are
 * controlled by the presenter (MVP). To be independent of the concrete implementation of model and view
 * this class doesn't create any of these objects. They must be set from outside using the set-Methods.
 * This allows to simply change the view class simply by providing other implementations of the given
 * interfaces. The model doesn't use interfaces yet, but it's set from outside too. So it should be
 * not too much work to change this.
 */

class GrubCustomizer : public ControllerAbstract, public GrubCustomizerIface {
	Model_Env& env;
	Model_ListCfg* grublistCfg;
	Model_Installer* installer;
	View_Installer* installDlg;
	Model_MountTable* mountTable;
	View_About* aboutDialog;
	ThreadController* threadController;
	Mapper_EntryName* entryNameMapper;

public:
	enum Exception {
		INCOMPLETE
	};
	void setListCfg(Model_ListCfg& grublistCfg);
	void setInstaller(Model_Installer& installer);
	void setInstallDlg(View_Installer& installDlg);
	void setMountTable(Model_MountTable& mountTable);
	void setAboutDialog(View_About& aboutDialog);
	void setThreadController(ThreadController& threadController);
	void setEntryNameMapper(Mapper_EntryName& mapper);

	ThreadController& getThreadController();

	GrubCustomizer(Model_Env& env);
	
	void showInstallDialog();
	void installGrub(std::string device);
	void showMessageGrubInstallCompleted(std::string const& msg);
	
	void showAboutDialog();
};

#endif
