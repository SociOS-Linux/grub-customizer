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
#include "../Model/Env.h"
#include "../View/Gtk/Main.h"
#include "../View/Gtk/About.h"
#include "../View/Gtk/Installer.h"
#include "../View/Gtk/Trash.h"
#include "../View/Gtk/EntryEditor.h"
#include "../View/Gtk/Settings.h"
#include "../View/Gtk/EnvEditor.h"
#include "glibMutex.h"
#include "glibThreadController.h"
#include "streamLogger.h"
#include <iostream>
#include "../lib/contentParser/FactoryImpl.h"
#include "../lib/contentParser/Linux.h"
#include "../lib/contentParser/LinuxIso.h"
#include "../lib/contentParser/Chainloader.h"
#include "../lib/contentParser/Memtest.h"


int main(int argc, char** argv){
	if (getuid() != 0 && (argc == 1 || argv[1] != std::string("no-fork"))) {
		return system((std::string("pkexec ") + argv[0] + (argc == 2 ? std::string(" ") + argv[1] : "") + " no-fork").c_str());
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
	EntryAddDlgGtk scriptAddDlg;
	EntryEditDlgGtk entryEditDlg;
	MountTable mountTable;
	GrublistCfg savedListCfg(env);
	FbResolutionsGetter fbResolutionsGetter;
	GrubSettingsDlgGtk settingsDlg;
	DeviceDataList deviceDataList;
	AboutDialogGtk aboutDialog;
	GlibMutex listCfgMutex1;
	GlibMutex listCfgMutex2;
	GlibThreadController threadC(presenter);
	ContentParserFactoryImpl contentParserFactory;
	GrubEnvEditorGtk envEditor;

	entryEditDlg.setDeviceDataList(deviceDataList);
	envEditor.setDeviceDataList(deviceDataList);

	//assign objects to presenter
	presenter.setListCfg(listcfg);
	presenter.setListCfgDlg(listCfgView);
	presenter.setSettingsDialog(settingsDlg);
	presenter.setSettingsManager(settings);
	presenter.setSettingsBuffer(settingsOnDisk);
	presenter.setInstaller(installer);
	presenter.setInstallDlg(installDlg);
	presenter.setScriptAddDlg(scriptAddDlg);
	presenter.setEntryEditDlg(entryEditDlg);
	presenter.setSavedListCfg(savedListCfg);
	presenter.setFbResolutionsGetter(fbResolutionsGetter);
	presenter.setDeviceDataList(deviceDataList);
	presenter.setMountTable(mountTable);
	presenter.setAboutDialog(aboutDialog);
	presenter.setThreadController(threadC);
	presenter.setContentParserFactory(contentParserFactory);
	presenter.setGrubEnvEditor(envEditor);

	listCfgView.putSettingsDialog(settingsDlg.getCommonSettingsPane(), settingsDlg.getAppearanceSettingsPane());

	//assign event listener
	EventListener evt(presenter);
	listCfgView.setEventListener(evt);
	installDlg.setEventListener(evt);
	scriptAddDlg.setEventListener(evt);
	entryEditDlg.setEventListener(evt);
	settingsDlg.setEventListener(evt);
	listcfg.setEventListener(evt);
	installer.setEventListener(evt);
	fbResolutionsGetter.setEventListener(evt);
	envEditor.setEventListener(evt);
	
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
	entryEditDlg.setLogger(logger);
	mountTable.setLogger(logger);
	savedListCfg.setLogger(logger);
	fbResolutionsGetter.setLogger(logger);
	settingsDlg.setLogger(logger);
	deviceDataList.setLogger(logger);
	aboutDialog.setLogger(logger);
	listCfgMutex1.setLogger(logger);
	listCfgMutex2.setLogger(logger);
	threadC.setLogger(logger);
	env.setLogger(logger);
	envEditor.setLogger(logger);

	// configure logger
	logger.setLogLevel(StreamLogger::LOG_EVENT);
	if (argc > 1) {
		std::string logParam = argv[1];
		if (logParam == "debug") {
			logger.setLogLevel(StreamLogger::LOG_DEBUG_ONLY);
		} else if (logParam == "log-important") {
			logger.setLogLevel(StreamLogger::LOG_IMPORTANT);
		} else if (logParam == "quiet") {
			logger.setLogLevel(StreamLogger::LOG_NOTHING);
		} else if (logParam == "verbose") {
			logger.setLogLevel(StreamLogger::LOG_VERBOSE);
		}
	}

	//configure contentParser factory
	GrubDeviceMap deviceMap(env);
	ContentParserLinux linuxParser(deviceMap);
	ContentParserLinuxIso linuxIsoParser(deviceMap);
	ContentParserChainloader chainloadParser(deviceMap);
	ContentParserMemtest memtestParser(deviceMap);

	contentParserFactory.registerParser(linuxParser, gettext("Linux"));
	contentParserFactory.registerParser(linuxIsoParser, gettext("Linux-ISO"));
	contentParserFactory.registerParser(chainloadParser, gettext("Chainloader"));
	contentParserFactory.registerParser(memtestParser, gettext("Memtest"));

	entryEditDlg.setAvailableEntryTypes(contentParserFactory.getNames());

	listcfg.setMutex(listCfgMutex1);
	savedListCfg.setMutex(listCfgMutex2);

	presenter.init();
	app.run();
}

