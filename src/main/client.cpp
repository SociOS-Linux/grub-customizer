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

#include <iostream>

#include "../Bootstrap/View.hpp"
#include "../Bootstrap/Application.hpp"
#include "../Bootstrap/Factory.hpp"
#include "../Model/ContentParser/Chainloader.hpp"
#include "../Model/ContentParser/FactoryImpl.hpp"
#include "../Model/ContentParser/Linux.hpp"
#include "../Model/ContentParser/LinuxIso.hpp"
#include "../Model/ContentParser/Memtest.hpp"
#include "../Controller/Helper/RuleMover/Strategy/MoveRuleOnSameLevelInsideProxy.hpp"
#include "../Controller/Helper/RuleMover/Strategy/MoveRuleIntoSubmenu.hpp"
#include "../Controller/Helper/RuleMover/Strategy/MoveRuleOutOfSubmenu.hpp"
#include "../Controller/Helper/RuleMover/Strategy/MoveRuleOutOfProxyOnToplevel.hpp"
#include "../Controller/Helper/RuleMover/Strategy/MoveRuleIntoForeignSubmenu.hpp"
#include "../Controller/Helper/RuleMover/Strategy/MoveForeignRuleFromSubmenuToToplevel.hpp"
#include "../Model/Logger/Stream.hpp"
#include "../View/Mapper/EntryNameImpl.hpp"
#include "../config.hpp"
#include "../Controller/AboutController.hpp"
#include "../Controller/EntryEditController.hpp"
#include "../Controller/EnvEditorController.hpp"
#include "../Controller/ErrorController.hpp"
#include "../Controller/InstallerController.hpp"
#include "../Controller/MainController.hpp"
#include "../Controller/SettingsController.hpp"
#include "../Controller/ThemeController.hpp"
#include "../Controller/TrashController.hpp"



int main(int argc, char** argv){
	if (getuid() != 0 && (argc == 1 || argv[1] != std::string("no-fork"))) {
		return system((std::string("pkexec ") + argv[0] + (argc == 2 ? std::string(" ") + argv[1] : "") + " no-fork").c_str());
	}
	setlocale(LC_ALL, "");
	bindtextdomain("grub-customizer", LOCALEDIR);
	textdomain("grub-customizer");

	auto logger = std::make_shared<Gc::Model::Logger::Stream>(std::cout);

	Gc::Model::Logger::GenericLogger::getInstance() = logger;

	try {
		auto application          = std::make_shared<Gc::Bootstrap::Application>(argc, argv);
		auto view                 = std::make_shared<Gc::Bootstrap::View>();
		auto factory              = std::make_shared<Gc::Bootstrap::Factory>(application->applicationObject, logger);

		auto settingsOnDisk       = factory->create<Gc::Model::SettingsManagerData>();
		auto savedListCfg         = factory->create<Gc::Model::ListCfg::ListCfg>();

		factory->entryNameMapper->setView(view->main);

		auto entryEditController = factory->createController<Gc::Controller::EntryEditController>(view->entryEditor);
		auto mainController      = factory->createController<Gc::Controller::MainController>(view->main);
		auto settingsController  = factory->createController<Gc::Controller::SettingsController>(view->settings);
		auto envEditController   = factory->createController<Gc::Controller::EnvEditorController>(view->envEditor);
		auto trashController     = factory->createController<Gc::Controller::TrashController>(view->trash);
		auto installController   = factory->createController<Gc::Controller::InstallerController>(view->installer);
		auto aboutController     = factory->createController<Gc::Controller::AboutController>(view->about);
		auto errorController     = factory->createController<Gc::Controller::ErrorController>(view->error);
		auto themeController     = factory->createController<Gc::Controller::ThemeController>(view->theme);

		mainController->setSettingsBuffer(settingsOnDisk);
		mainController->setSavedListCfg(savedListCfg);

		// configure logger
		logger->setLogLevel(Gc::Model::Logger::Stream::LOG_EVENT);
		if (argc > 1) {
			std::string logParam = argv[1];
			if (logParam == "debug") {
				logger->setLogLevel(Gc::Model::Logger::Stream::LOG_DEBUG_ONLY);
			} else if (logParam == "log-important") {
				logger->setLogLevel(Gc::Model::Logger::Stream::LOG_IMPORTANT);
			} else if (logParam == "quiet") {
				logger->setLogLevel(Gc::Model::Logger::Stream::LOG_NOTHING);
			} else if (logParam == "verbose") {
				logger->setLogLevel(Gc::Model::Logger::Stream::LOG_VERBOSE);
			}
		}

		factory->contentParserFactory->registerParser(factory->create<Gc::Model::ContentParser::Linux>(), gettext("Linux"));
		factory->contentParserFactory->registerParser(factory->create<Gc::Model::ContentParser::LinuxIso>(), gettext("Linux-ISO"));
		factory->contentParserFactory->registerParser(factory->create<Gc::Model::ContentParser::Chainloader>(), gettext("Chainloader"));
		factory->contentParserFactory->registerParser(factory->create<Gc::Model::ContentParser::Memtest>(), gettext("Memtest"));

		view->entryEditor->setAvailableEntryTypes(factory->contentParserFactory->getNames());

		factory->ruleMover->addStrategy(factory->create<Gc::Controller::Helper::RuleMover::Strategy::MoveRuleIntoSubmenu>());
		factory->ruleMover->addStrategy(factory->create<Gc::Controller::Helper::RuleMover::Strategy::MoveRuleOnSameLevelInsideProxy>());
		factory->ruleMover->addStrategy(factory->create<Gc::Controller::Helper::RuleMover::Strategy::MoveForeignRuleFromSubmenuToToplevel>());
		factory->ruleMover->addStrategy(factory->create<Gc::Controller::Helper::RuleMover::Strategy::MoveRuleOutOfSubmenu>());
		factory->ruleMover->addStrategy(factory->create<Gc::Controller::Helper::RuleMover::Strategy::MoveRuleIntoForeignSubmenu>());
		factory->ruleMover->addStrategy(factory->create<Gc::Controller::Helper::RuleMover::Strategy::MoveRuleOutOfProxyOnToplevel>());

		mainController->initAction();
		errorController->setApplicationStarted(true);

		application->applicationObject->run();
	} catch (Gc::Common::Exception::GenericException const& e) {
		logger->log(e, Gc::Model::Logger::GenericLogger::ERROR);
		return 1;
	}
}

