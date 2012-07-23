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

#include "grubCustomizer.h"
#include "eventListener.h"
#include "../model/grubEnv.h"
#include "../view/grublistCfgDlgGtk.h"
#include "../view/aboutDialogGtk.h"
#include "../view/grubInstallDlgGtk.h"
#include "../view/partitionChooserGtk.h"
#include "../view/scriptAddDlgGtk.h"
#include "../view/settingsDlgGtk.h"
#include "glibMutex.h"
#include "glibThreadController.h"
#include "streamLogger.h"
#include <iostream>

int main(int argc, char** argv){
	if (getuid() != 0 && (argc == 1 || argv[1] != std::string("no-fork"))) {
		return system((std::string("pkexec ") + argv[0] + " no-fork").c_str());
	}
	setlocale( LC_ALL, "");
	bindtextdomain( "grub-customizer", LOCALEDIR);
	textdomain( "grub-customizer" );

	Gtk::Main app(argc, argv);
	Glib::thread_init();

	GrubEnv env;

	GrubCustomizer presenter(env);

	GrublistCfg listcfg(env);
	GrublistCfgDlgGtk listCfgView;
	SettingsManagerDataStore settings(env);
	SettingsManagerDataStore settingsOnDisk(env);
	GrubInstaller installer(env);
	GrubInstallDlgGtk installDlg;
	ScriptAddDlgGtk scriptAddDlg;
	MountTable mountTable;
	PartitionChooserGtk partitionChooser;
	GrublistCfg savedListCfg(env);
	FbResolutionsGetter fbResolutionsGetter;
	GrubSettingsDlgGtk settingsDlg;
	DeviceDataList deviceDataList;
	AboutDialogGtk aboutDialog;
	GlibMutex listCfgMutex1;
	GlibMutex listCfgMutex2;
	GlibThreadController threadC(presenter);

	//assign objects to presenter
	presenter.setListCfg(listcfg);
	presenter.setListCfgDlg(listCfgView);
	presenter.setSettingsDialog(settingsDlg);
	presenter.setSettingsManager(settings);
	presenter.setSettingsBuffer(settingsOnDisk);
	presenter.setInstaller(installer);
	presenter.setInstallDlg(installDlg);
	presenter.setScriptAddDlg(scriptAddDlg);
	presenter.setPartitionChooser(partitionChooser);
	presenter.setSavedListCfg(savedListCfg);
	presenter.setFbResolutionsGetter(fbResolutionsGetter);
	presenter.setDeviceDataList(deviceDataList);
	presenter.setMountTable(mountTable);
	presenter.setAboutDialog(aboutDialog);
	presenter.setThreadController(threadC);

	listCfgView.putSettingsDialog(settingsDlg.getCommonSettingsPane(), settingsDlg.getAppearanceSettingsPane());

	//assign event listener
	EventListener evt(presenter);
	listCfgView.setEventListener(evt);
	installDlg.setEventListener(evt);
	scriptAddDlg.setEventListener(evt);
	settingsDlg.setEventListener(evt);
	partitionChooser.setEventListener(evt);
	listcfg.setEventListener(evt);
	installer.setEventListener(evt);
	fbResolutionsGetter.setEventListener(evt);
	
	//assign logger
	StreamLogger logger(std::cout);
	presenter.setLogger(logger);
	listcfg.setLogger(logger);
	listCfgView.setLogger(logger);
	settings.setLogger(logger);
	settingsOnDisk.setLogger(logger);
	installer.setLogger(logger);
	installDlg.setLogger(logger);
	scriptAddDlg.setLogger(logger);
	mountTable.setLogger(logger);
	partitionChooser.setLogger(logger);
	savedListCfg.setLogger(logger);
	fbResolutionsGetter.setLogger(logger);
	settingsDlg.setLogger(logger);
	deviceDataList.setLogger(logger);
	aboutDialog.setLogger(logger);
	listCfgMutex1.setLogger(logger);
	listCfgMutex2.setLogger(logger);
	threadC.setLogger(logger);
	env.setLogger(logger);


	
	listcfg.setMutex(listCfgMutex1);
	savedListCfg.setMutex(listCfgMutex2);

	presenter.init();
	app.run();
}

