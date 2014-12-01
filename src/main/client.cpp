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

#include "../Model/Env.cpp"
#include "../lib/Logger/Stream.cpp"
#include <iostream>
#include "../lib/ContentParser/FactoryImpl.cpp"
#include "../lib/ContentParser/Linux.cpp"
#include "../lib/ContentParser/LinuxIso.cpp"
#include "../lib/ContentParser/Chainloader.cpp"
#include "../lib/ContentParser/Memtest.cpp"
#include "../Controller/EntryEditControllerImpl.cpp"
#include "../Controller/MainControllerImpl.cpp"
#include "../Controller/SettingsControllerImpl.cpp"
#include "../Controller/EnvEditorControllerImpl.cpp"
#include "../Controller/TrashControllerImpl.cpp"
#include "../Controller/InstallerControllerImpl.cpp"
#include "../Controller/AboutControllerImpl.cpp"
#include "../Controller/ControllerCollection.h"
#include "../Controller/ErrorControllerImpl.cpp"
#include "../Controller/ThemeControllerImpl.cpp"
#include "../Mapper/EntryNameImpl.cpp"
#include "../lib/ArrayStructure.cpp"
#include "../Model/ThemeManager.cpp"

#include "../Bootstrap/View.h"
#include "../Bootstrap/Thread.h"
#include "../Bootstrap/Regex.h"


int main(int argc, char** argv){
	if (getuid() != 0 && (argc == 1 || argv[1] != std::string("no-fork"))) {
		return system((std::string("pkexec ") + argv[0] + (argc == 2 ? std::string(" ") + argv[1] : "") + " no-fork").c_str());
	}
	setlocale( LC_ALL, "");
	bindtextdomain( "grub-customizer", LOCALEDIR);
	textdomain( "grub-customizer" );

	Logger_Stream logger(std::cout);

	try {
		Model_Env env;

		Bootstrap_View view(argc, argv);
		Bootstrap_Thread thread;
		Bootstrap_Regex regex;

		Model_ListCfg listcfg;
		Model_SettingsManagerData settings;
		Model_SettingsManagerData settingsOnDisk;
		Model_Installer installer;
		Model_MountTable mountTable;
		Model_ListCfg savedListCfg;
		Model_FbResolutionsGetter fbResolutionsGetter;
		Model_DeviceDataList deviceDataList;
		ContentParser_FactoryImpl contentParserFactory;
		Mapper_EntryNameImpl entryNameMapper;
		Model_ThemeManager themeManager;
		Model_DeviceMap deviceMap;

		deviceMap.setRegexEngine(*regex.engine);

		entryNameMapper.setView(*view.main);

		view.setDeviceDataList(deviceDataList);

		EntryEditControllerImpl entryEditController;
		entryEditController.setContentParserFactory(contentParserFactory);
		entryEditController.setView(*view.entryEditor);
		entryEditController.setDeviceDataList(deviceDataList);
		entryEditController.setListCfg(listcfg);

		MainControllerImpl mainController;
		mainController.setListCfg(listcfg);
		mainController.setSettingsManager(settings);
		mainController.setSettingsBuffer(settingsOnDisk);
		mainController.setSavedListCfg(savedListCfg);
		mainController.setFbResolutionsGetter(fbResolutionsGetter);
		mainController.setDeviceDataList(deviceDataList);
		mainController.setMountTable(mountTable);
		mainController.setContentParserFactory(contentParserFactory);
		mainController.setView(*view.main);
		mainController.setEntryNameMapper(entryNameMapper);

		SettingsControllerImpl settingsController;
		settingsController.setListCfg(listcfg);
		settingsController.setView(*view.settings);
		settingsController.setSettingsManager(settings);
		settingsController.setFbResolutionsGetter(fbResolutionsGetter);

		EnvEditorControllerImpl envEditController;
		envEditController.setMountTable(mountTable);
		envEditController.setView(*view.envEditor);
		envEditController.setDeviceMap(deviceMap);

		TrashControllerImpl trashController;
		trashController.setEntryNameMapper(entryNameMapper);
		trashController.setListCfg(listcfg);
		trashController.setDeviceDataList(deviceDataList);
		trashController.setContentParserFactory(contentParserFactory);
		trashController.setView(*view.trash);

		InstallerControllerImpl installController;
		installController.setInstaller(installer);
		installController.setView(*view.installer);

		AboutControllerImpl aboutController;
		aboutController.setView(*view.about);

		ErrorControllerImpl errorController;
		errorController.setView(*view.error);

		ThemeControllerImpl themeController;
		themeController.setView(*view.theme);
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

		dynamic_cast<ControllerCollection_Connection&>(*thread.threadController).setControllerCollection(controllerCollection);
		entryEditController.setControllerCollection(controllerCollection);
		mainController.setControllerCollection(controllerCollection);
		settingsController.setControllerCollection(controllerCollection);
		envEditController.setControllerCollection(controllerCollection);
		trashController.setControllerCollection(controllerCollection);
		installController.setControllerCollection(controllerCollection);
		aboutController.setControllerCollection(controllerCollection);
		errorController.setControllerCollection(controllerCollection);
		themeController.setControllerCollection(controllerCollection);

		mainController.setThreadController(*thread.threadController);
		settingsController.setThreadController(*thread.threadController);
		installController.setThreadController(*thread.threadController);
		errorController.setThreadController(*thread.threadController);
		entryEditController.setThreadController(*thread.threadController);
		themeController.setThreadController(*thread.threadController);

		//assign event listener
		view.main->setController(mainController);
		view.installer->setController(installController);
		view.trash->setController(trashController);
		view.entryEditor->setController(entryEditController);
		view.settings->setController(settingsController);
		listcfg.setController(mainController);
		installer.setController(installController);
		fbResolutionsGetter.setController(settingsController);
		view.envEditor->setController(envEditController);
		view.error->setController(errorController);
		view.theme->setController(themeController);

		//assign logger
		listcfg.setLogger(logger);
		view.main->setLogger(logger);
		settings.setLogger(logger);
		settingsOnDisk.setLogger(logger);
		installer.setLogger(logger);
		view.installer->setLogger(logger);
		view.trash->setLogger(logger);
		view.entryEditor->setLogger(logger);
		mountTable.setLogger(logger);
		savedListCfg.setLogger(logger);
		fbResolutionsGetter.setLogger(logger);
		view.settings->setLogger(logger);
		deviceDataList.setLogger(logger);
		view.about->setLogger(logger);
		thread.mutex1->setLogger(logger);
		thread.mutex2->setLogger(logger);
		thread.threadController->setLogger(logger);
		env.setLogger(logger);
		view.envEditor->setLogger(logger);
		mainController.setLogger(logger);
		entryEditController.setLogger(logger);
		settingsController.setLogger(logger);
		envEditController.setLogger(logger);
		trashController.setLogger(logger);
		errorController.setLogger(logger);
		installController.setLogger(logger);
		aboutController.setLogger(logger);
		view.theme->setLogger(logger);
		themeController.setLogger(logger);

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
		ContentParser_Linux linuxParser(deviceMap);
		ContentParser_LinuxIso linuxIsoParser(deviceMap);
		ContentParser_Chainloader chainloadParser(deviceMap);
		ContentParser_Memtest memtestParser(deviceMap);

		contentParserFactory.registerParser(linuxParser, gettext("Linux"));
		contentParserFactory.registerParser(linuxIsoParser, gettext("Linux-ISO"));
		contentParserFactory.registerParser(chainloadParser, gettext("Chainloader"));
		contentParserFactory.registerParser(memtestParser, gettext("Memtest"));

		linuxParser.setRegexEngine(*regex.engine);
		linuxIsoParser.setRegexEngine(*regex.engine);
		chainloadParser.setRegexEngine(*regex.engine);
		memtestParser.setRegexEngine(*regex.engine);

		view.entryEditor->setAvailableEntryTypes(contentParserFactory.getNames());

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
		listcfg.setMutex(*thread.mutex1);
		savedListCfg.setMutex(*thread.mutex2);

		mainController.initAction();
		errorController.setApplicationStarted(true);

		view.run();
	} catch (Exception const& e) {
		logger.log(e, Logger::ERROR);
		return 1;
	}
}

