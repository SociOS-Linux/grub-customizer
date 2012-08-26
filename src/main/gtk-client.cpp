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

#include "../presenter/grubCustomizer.h"
#include "../presenter/eventListener.h"
#include "../Model/Env.h"
#include "../View/Gtk/Main.h"
#include "../View/Gtk/About.h"
#include "../View/Gtk/Installer.h"
#include "../View/Gtk/Trash.h"
#include "../View/Gtk/EntryEditor.h"
#include "../View/Gtk/Settings.h"
#include "../View/Gtk/EnvEditor.h"
#include "../presenter/glibMutex.h"
#include "../presenter/glibThreadController.h"
#include "../presenter/streamLogger.h"
#include <iostream>
#include "../lib/contentParser/FactoryImpl.h"
#include "../lib/contentParser/Linux.h"
#include "../lib/contentParser/LinuxIso.h"
#include "../lib/contentParser/Chainloader.h"
#include "../lib/contentParser/Memtest.h"
#include "../Controller/EntryEditControllerImpl.h"
#include "../Controller/MainControllerImpl.h"
#include "../Controller/SettingsControllerImpl.h"
#include "../Controller/EnvEditorControllerImpl.h"
#include "../Controller/TrashControllerImpl.h"
#include "../ControllerCollection.h"
#include "../Mapper/EntryNameImpl.h"


int main(int argc, char** argv){
	if (getuid() != 0 && (argc == 1 || argv[1] != std::string("no-fork"))) {
		return system((std::string("pkexec ") + argv[0] + (argc == 2 ? std::string(" ") + argv[1] : "") + " no-fork").c_str());
	}
	setlocale( LC_ALL, "");
	bindtextdomain( "grub-customizer", LOCALEDIR);
	textdomain( "grub-customizer" );

	Gtk::Main app(argc, argv);


	Glib::thread_init();

	Model_Env env;

	GrubCustomizer presenter(env);

	Model_ListCfg listcfg(env);
	View_Gtk_Main listCfgView;
	Model_SettingsManagerData settings(env);
	Model_SettingsManagerData settingsOnDisk(env);
	Model_Installer installer(env);
	View_Gtk_Installer installDlg;
	View_Gtk_Trash trashView;
	View_Gtk_EntryEditor entryEditDlg;
	Model_MountTable mountTable;
	Model_ListCfg savedListCfg(env);
	Model_FbResolutionsGetter fbResolutionsGetter;
	View_Gtk_Settings settingsDlg;
	Model_DeviceDataList deviceDataList;
	View_Gtk_About aboutDialog;
	GlibMutex listCfgMutex1;
	GlibMutex listCfgMutex2;
	ContentParserFactoryImpl contentParserFactory;
	View_Gtk_EnvEditor envEditor;
	Mapper_EntryNameImpl entryNameMapper;

	entryNameMapper.setView(listCfgView);

	entryEditDlg.setDeviceDataList(deviceDataList);
	envEditor.setDeviceDataList(deviceDataList);

	EntryEditControllerImpl entryEditController(env);
	entryEditController.setContentParserFactory(contentParserFactory);
	entryEditController.setView(entryEditDlg);
	entryEditController.setDeviceDataList(deviceDataList);
	entryEditController.setListCfg(listcfg);

	MainControllerImpl mainController(env);
	mainController.setListCfg(listcfg);
	mainController.setSettingsManager(settings);
	mainController.setSettingsBuffer(settingsOnDisk);
	mainController.setSavedListCfg(savedListCfg);
	mainController.setFbResolutionsGetter(fbResolutionsGetter);
	mainController.setDeviceDataList(deviceDataList);
	mainController.setMountTable(mountTable);
	mainController.setContentParserFactory(contentParserFactory);
	mainController.setView(listCfgView);
	mainController.setEntryNameMapper(entryNameMapper);

	SettingsControllerImpl settingsController(env);
	settingsController.setListCfg(listcfg);
	settingsController.setSettingsDialog(settingsDlg);
	settingsController.setSettingsManager(settings);
	settingsController.setSettingsBuffer(settingsOnDisk);
	settingsController.setFbResolutionsGetter(fbResolutionsGetter);

	EnvEditorControllerImpl envEditController(env);
	envEditController.setMountTable(mountTable);
	envEditController.setView(envEditor);

	TrashControllerImpl trashController(env);
	trashController.setEntryNameMapper(entryNameMapper);
	trashController.setListCfg(listcfg);
	trashController.setView(trashView);

	ControllerCollection controllerCollection;
	controllerCollection.entryEditController = &entryEditController;
	controllerCollection.mainController = &mainController;
	controllerCollection.settingsController = &settingsController;
	controllerCollection.envEditController = &envEditController;
	controllerCollection.trashController = &trashController;
	controllerCollection.masterclass_deprecated = &presenter;

	entryEditController.setControllerCollection(controllerCollection);
	mainController.setControllerCollection(controllerCollection);
	presenter.setControllerCollection(controllerCollection);
	settingsController.setControllerCollection(controllerCollection);
	envEditController.setControllerCollection(controllerCollection);
	trashController.setControllerCollection(controllerCollection);

	GlibThreadController threadC(presenter, controllerCollection);
	mainController.setThreadController(threadC);
	settingsController.setThreadController(threadC);

	//assign objects to presenter
	presenter.setListCfg(listcfg);
	presenter.setInstaller(installer);
	presenter.setInstallDlg(installDlg);
	presenter.setMountTable(mountTable);
	presenter.setAboutDialog(aboutDialog);
	presenter.setThreadController(threadC);
	presenter.setEntryNameMapper(entryNameMapper);

	listCfgView.putSettingsDialog(settingsDlg.getCommonSettingsPane(), settingsDlg.getAppearanceSettingsPane());

	//assign event listener
	EventListener evt(presenter, controllerCollection);
	listCfgView.setEventListener(mainController);
	installDlg.setEventListener(evt);
	trashView.setEventListener(trashController);
	entryEditDlg.setEventListener(entryEditController);
	settingsDlg.setEventListener(settingsController);
	listcfg.setEventListener(evt);
	installer.setEventListener(evt);
	fbResolutionsGetter.setEventListener(evt);
	envEditor.setEventListener(envEditController);
	
	//assign logger
	StreamLogger logger(std::cout);
	presenter.setLogger(logger);
	listcfg.setLogger(logger);
	listCfgView.setLogger(logger);
	settings.setLogger(logger);
	settingsOnDisk.setLogger(logger);
	installer.setLogger(logger);
	installDlg.setLogger(logger);
	trashView.setLogger(logger);
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
	mainController.setLogger(logger);
	settingsController.setLogger(logger);
	envEditController.setLogger(logger);
	trashController.setLogger(logger);

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
	Model_DeviceMap deviceMap(env);
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

	mainController.init();
	app.run();
}

