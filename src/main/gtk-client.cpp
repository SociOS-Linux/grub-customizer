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

#include "../Model/Env.h"
#include "../View/Gtk/Main.h"
#include "../View/Gtk/About.h"
#include "../View/Gtk/Installer.h"
#include "../View/Gtk/Trash.h"
#include "../View/Gtk/EntryEditor.h"
#include "../View/Gtk/Settings.h"
#include "../View/Gtk/EnvEditor.h"
#include "../View/Gtk/Error.h"
#include "../View/Gtk/Theme.h"
#include "../lib/Mutex/GLib.h"
#include "../Controller/GLib/ThreadController.h"
#include "../lib/Logger/Stream.h"
#include <iostream>
#include "../lib/ContentParser/FactoryImpl.h"
#include "../lib/ContentParser/Linux.h"
#include "../lib/ContentParser/LinuxIso.h"
#include "../lib/ContentParser/Chainloader.h"
#include "../lib/ContentParser/Memtest.h"
#include "../Controller/EntryEditControllerImpl.h"
#include "../Controller/MainControllerImpl.h"
#include "../Controller/SettingsControllerImpl.h"
#include "../Controller/EnvEditorControllerImpl.h"
#include "../Controller/TrashControllerImpl.h"
#include "../Controller/InstallerControllerImpl.h"
#include "../Controller/AboutControllerImpl.h"
#include "../Controller/ControllerCollection.h"
#include "../Controller/ErrorControllerImpl.h"
#include "../Controller/ThemeControllerImpl.h"
#include "../Mapper/EntryNameImpl.h"
#include "../lib/assert.h"
#include "../lib/ArrayStructure.h"
#include "../Model/ThemeManager.h"
#include "../lib/Bootstrap.h"
#include "../lib/Trait/LoggerConnector.h"
#include "../View/Trait/ViewConnector.h"


int main(int argc, char** argv){
	if (getuid() != 0 && (argc == 1 || argv[1] != std::string("no-fork"))) {
		return system((std::string("pkexec ") + argv[0] + (argc == 2 ? std::string(" ") + argv[1] : "") + " no-fork").c_str());
	}
	setlocale( LC_ALL, "");
	bindtextdomain( "grub-customizer", LOCALEDIR);
	textdomain( "grub-customizer" );

	Logger_Stream logger(std::cout);

	try {
		Gtk::Main app(argc, argv);

		Glib::thread_init();

		Bootstrap bootstrap;

		Logger_Connector loggerConnector(logger);
		bootstrap.pushRessource(loggerConnector);

		Model_Env env;
		bootstrap << env;

		Model_ListCfg listcfg;
		Model_SettingsManagerData settings;
		Model_SettingsManagerData settingsOnDisk;
		Model_Installer installer;
		Model_MountTable mountTable;
		Model_ListCfg savedListCfg;
		Model_FbResolutionsGetter fbResolutionsGetter;
		Model_DeviceDataList deviceDataList;
		Mutex_GLib listCfgMutex1;
		Mutex_GLib listCfgMutex2;
		ContentParser_FactoryImpl contentParserFactory;
		Mapper_EntryNameImpl entryNameMapper;
		Model_ThemeManager themeManager;
		View_Gtk_Main listCfgView;
		View_Gtk_Installer installDlg;
		View_Gtk_Trash trashView;
		View_Gtk_EntryEditor entryEditDlg;
		View_Gtk_Settings settingsDlg;
		View_Gtk_About aboutDialog;
		View_Gtk_EnvEditor envEditor;
		View_Gtk_Error errorView;
		View_Gtk_Theme themeEditor;

		View_Trait_ViewConnector<View_Main> listCfgViewCnn(listCfgView);
		bootstrap.pushRessource(listCfgViewCnn);

		View_Trait_ViewConnector<View_Installer> installDlgCnn(installDlg);
		bootstrap.pushRessource(installDlgCnn);

		View_Trait_ViewConnector<View_Trash> trashViewCnn(trashView);
		bootstrap.pushRessource(trashViewCnn);

		View_Trait_ViewConnector<View_EntryEditor> entryEditDlgCnn(entryEditDlg);
		bootstrap.pushRessource(entryEditDlgCnn);

		View_Trait_ViewConnector<View_Settings> settingsDlgCnn(settingsDlg);
		bootstrap.pushRessource(settingsDlgCnn);

		View_Trait_ViewConnector<View_About> aboutDialogCnn(aboutDialog);
		bootstrap.pushRessource(aboutDialogCnn);

		View_Trait_ViewConnector<View_EnvEditor> envEditorCnn(envEditor);
		bootstrap.pushRessource(envEditorCnn);

		View_Trait_ViewConnector<View_Error> errorViewCnn(errorView);
		bootstrap.pushRessource(errorViewCnn);

		View_Trait_ViewConnector<View_Theme> themeEditorCnn(themeEditor);
		bootstrap.pushRessource(themeEditorCnn);


		bootstrap << listcfg
		          << listCfgView
		          << settings
		          << settingsOnDisk
		          << installer
		          << installDlg
		          << trashView
		          << entryEditDlg
		          << mountTable
		          << savedListCfg
		          << fbResolutionsGetter
		          << settingsDlg
		          << deviceDataList
		          << aboutDialog
		          << listCfgMutex1
		          << listCfgMutex2
		          << contentParserFactory
		          << envEditor
		          << errorView
		          << entryNameMapper
		          << themeEditor
		          << themeManager;

		entryEditDlg.setDeviceDataList(deviceDataList);
		envEditor.setDeviceDataList(deviceDataList);

		EntryEditControllerImpl entryEditController;
		bootstrap << entryEditController;
		entryEditController.setContentParserFactory(contentParserFactory);
		entryEditController.setDeviceDataList(deviceDataList);
		entryEditController.setListCfg(listcfg);

		MainControllerImpl mainController;
		bootstrap << mainController;
		mainController.setListCfg(listcfg);
		mainController.setSettingsManager(settings);
		mainController.setSettingsBuffer(settingsOnDisk);
		mainController.setSavedListCfg(savedListCfg);
		mainController.setFbResolutionsGetter(fbResolutionsGetter);
		mainController.setDeviceDataList(deviceDataList);
		mainController.setMountTable(mountTable);
		mainController.setContentParserFactory(contentParserFactory);
		mainController.setEntryNameMapper(entryNameMapper);

		SettingsControllerImpl settingsController;
		bootstrap << settingsController;
		settingsController.setListCfg(listcfg);
		settingsController.setSettingsManager(settings);
		settingsController.setFbResolutionsGetter(fbResolutionsGetter);

		EnvEditorControllerImpl envEditController;
		bootstrap << envEditController;
		envEditController.setMountTable(mountTable);

		TrashControllerImpl trashController;
		bootstrap << trashController;
		trashController.setEntryNameMapper(entryNameMapper);
		trashController.setListCfg(listcfg);
		trashController.setDeviceDataList(deviceDataList);
		trashController.setContentParserFactory(contentParserFactory);

		InstallerControllerImpl installController;
		bootstrap << installController;
		installController.setInstaller(installer);

		AboutControllerImpl aboutController;
		bootstrap << aboutController;

		ErrorControllerImpl errorController;
		bootstrap << errorController;

		ThemeControllerImpl themeController;
		bootstrap << themeController;
		themeController.setThemeManager(themeManager);
		themeController.setSettingsManager(settings);
		themeController.setListCfg(listcfg);

		ControllerCollection controllerCollection;
		controllerCollection.entryEditController = &entryEditController;
		controllerCollection.mainController = &mainController;
		controllerCollection.settingsController = &settingsController;
		controllerCollection.envEditController = &envEditController;
		controllerCollection.trashController = &trashController;
		controllerCollection.installerController = &installController;
		controllerCollection.aboutController = &aboutController;
		controllerCollection.errorController = &errorController;
		controllerCollection.themeController = &themeController;

		entryEditController.setControllerCollection(controllerCollection);
		mainController.setControllerCollection(controllerCollection);
		settingsController.setControllerCollection(controllerCollection);
		envEditController.setControllerCollection(controllerCollection);
		trashController.setControllerCollection(controllerCollection);
		installController.setControllerCollection(controllerCollection);
		aboutController.setControllerCollection(controllerCollection);
		errorController.setControllerCollection(controllerCollection);
		themeController.setControllerCollection(controllerCollection);

		GLib_ThreadController threadC(controllerCollection);
		bootstrap << threadC;
		mainController.setThreadController(threadC);
		settingsController.setThreadController(threadC);
		installController.setThreadController(threadC);
		errorController.setThreadController(threadC);
		entryEditController.setThreadController(threadC);
		themeController.setThreadController(threadC);

		listCfgView.putSettingsDialog(settingsDlg.getCommonSettingsPane(), settingsDlg.getAppearanceSettingsPane());
		listCfgView.putTrashList(trashView.getList());
		settingsDlg.putThemeSelector(themeEditor.getThemeSelector());
		settingsDlg.putThemeEditArea(themeEditor.getEditorBox());

		//assign event listener
		listCfgView.setController(mainController);
		installDlg.setController(installController);
		trashView.setController(trashController);
		entryEditDlg.setController(entryEditController);
		settingsDlg.setController(settingsController);
		listcfg.setController(mainController);
		installer.setController(installController);
		fbResolutionsGetter.setController(settingsController);
		envEditor.setController(envEditController);
		errorView.setController(errorController);
		themeEditor.setController(themeController);

		// configure logger
		logger.setLogLevel(Logger_Stream::LOG_EVENT);
		if (argc > 1) {
			std::string logParam = argv[1];
			if (logParam == "debug") {
				logger.setLogLevel(Logger_Stream::LOG_DEBUG_ONLY);
			} else if (logParam == "log-important") {
				logger.setLogLevel(Logger_Stream::LOG_IMPORTANT);
			} else if (logParam == "quiet") {
				logger.setLogLevel(Logger_Stream::LOG_NOTHING);
			} else if (logParam == "verbose") {
				logger.setLogLevel(Logger_Stream::LOG_VERBOSE);
			}
		}

		//configure contentParser factory
		Model_DeviceMap deviceMap;
		ContentParser_Linux linuxParser(deviceMap);
		ContentParser_LinuxIso linuxIsoParser(deviceMap);
		ContentParser_Chainloader chainloadParser(deviceMap);
		ContentParser_Memtest memtestParser(deviceMap);

		contentParserFactory.registerParser(linuxParser, gettext("Linux"));
		contentParserFactory.registerParser(linuxIsoParser, gettext("Linux-ISO"));
		contentParserFactory.registerParser(chainloadParser, gettext("Chainloader"));
		contentParserFactory.registerParser(memtestParser, gettext("Memtest"));

		entryEditDlg.setAvailableEntryTypes(contentParserFactory.getNames());

		//set env
		listcfg.setEnv(env);
		savedListCfg.setEnv(env);
		settings.setEnv(env);
		settingsOnDisk.setEnv(env);
		installer.setEnv(env);
		themeManager.setEnv(env);
		entryEditController.setEnv(env);
		mainController.setEnv(env);
		settingsController.setEnv(env);
		envEditController.setEnv(env);
		trashController.setEnv(env);
		installController.setEnv(env);
		themeController.setEnv(env);
		deviceMap.setEnv(env);

		//set mutex
		listcfg.setMutex(listCfgMutex1);
		savedListCfg.setMutex(listCfgMutex2);

		bootstrap.run();

		mainController.initAction();
		errorController.setApplicationStarted(true);
		app.run();
	} catch (Exception const& e) {
		logger.log(e, Logger::ERROR);
		return 1;
	}
}

