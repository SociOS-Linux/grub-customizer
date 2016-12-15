/*
 * Copyright (C) 2010-2014 Daniel Richter <danielrichter2007@web.de>
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

#ifndef SRC_BOOTSTRAP_FACTORY_HPP_
#define SRC_BOOTSTRAP_FACTORY_HPP_

#include "../View/Trait/ViewAware.hpp"
#include "../Model/Env.hpp"
#include "../Model/ListCfg/ListCfg.hpp"
#include "../Model/Device/MountTable.hpp"
#include "../Model/Device/MountTableConnection.hpp"
#include "../Model/SettingsManagerData.hpp"
#include "../Model/SettingsManagerDataConnection.hpp"
#include "../Model/Installer.hpp"
#include "../Model/InstallerConnection.hpp"
#include "../Model/FbResolutionsGetter.hpp"
#include "../Model/FbResolutionsGetterConnection.hpp"
#include "../Model/Device/DeviceDataListConnection.hpp"
#include "../Model/Device/DeviceDataListInterfaceConnection.hpp"
#include "../Model/ContentParser/FactoryImpl.hpp"
#include "../Model/ContentParser/GenericFactoryConnection.hpp"
#include "../View/Mapper/EntryNameImpl.hpp"
#include "../View/Mapper/EntryNameConnection.hpp"
#include "../Model/ThemeManager.hpp"
#include "../Model/ThemeManagerConnection.hpp"
#include "../Model/Device/DeviceMap.hpp"
#include "../Model/Device/DeviceMapConnection.hpp"
#include "../Controller/Helper/Thread.hpp"
#include "../Controller/Helper/ThreadConnection.hpp"
#include "../Controller/Helper/RuleMoverConnection.hpp"
#include "Application.hpp"
#include "ApplicationHelper/ObjectConnection.hpp"

namespace Gc { namespace Bootstrap { class Factory
{
	public: std::shared_ptr<Gc::Model::Env> env;
	public: std::shared_ptr<Gc::Model::ListCfg::ListCfg> listcfg;
	public: std::shared_ptr<Gc::Model::SettingsManagerData> settings;
	public: std::shared_ptr<Gc::Model::Installer> installer;
	public: std::shared_ptr<Gc::Model::Device::MountTable> mountTable;
	public: std::shared_ptr<Gc::Model::FbResolutionsGetter> fbResolutionsGetter;
	public: std::shared_ptr<Gc::Model::Device::DeviceDataList> deviceDataList;
	public: std::shared_ptr<Gc::Model::ContentParser::FactoryImpl> contentParserFactory;
	public: std::shared_ptr<Gc::View::Mapper::EntryNameImpl> entryNameMapper;
	public: std::shared_ptr<Gc::Model::ThemeManager> themeManager;
	public: std::shared_ptr<Gc::Model::Device::DeviceMap> deviceMap;
	public: std::shared_ptr<Gc::Controller::Helper::RuleMoverHelper> ruleMover;
	public: std::shared_ptr<Gc::Model::Logger::GenericLogger> logger;

	public: std::shared_ptr<Gc::Common::Regex::Generic> regexEngine;
	public: std::shared_ptr<Gc::Controller::Helper::Thread> threadHelper;

	public: std::shared_ptr<Gc::Bootstrap::ApplicationHelper::Object> applicationObject;

	public: Factory(std::shared_ptr<Gc::Bootstrap::ApplicationHelper::Object> applicationObject, std::shared_ptr<Gc::Model::Logger::GenericLogger> logger)
	{
		this->applicationObject    = applicationObject;
		this->logger               = logger;

		this->regexEngine          = this->createRegexExgine();
		this->threadHelper         = this->createThreadHelper();

		this->env                  = this->create<Gc::Model::Env>();
		this->listcfg              = this->create<Gc::Model::ListCfg::ListCfg>();
		this->settings             = this->create<Gc::Model::SettingsManagerData>();
		this->installer            = this->create<Gc::Model::Installer>();
		this->mountTable           = this->create<Gc::Model::Device::MountTable>();
		this->fbResolutionsGetter  = this->create<Gc::Model::FbResolutionsGetter>();
		this->deviceDataList       = this->create<Gc::Model::Device::DeviceDataList>();
		this->contentParserFactory = this->create<Gc::Model::ContentParser::FactoryImpl>();
		this->entryNameMapper      = this->create<Gc::View::Mapper::EntryNameImpl>();
		this->themeManager         = this->create<Gc::Model::ThemeManager>();
		this->deviceMap            = this->create<Gc::Model::Device::DeviceMap>();
		this->ruleMover            = this->create<Gc::Controller::Helper::RuleMoverHelper>();

		this->bootstrap(this->regexEngine);
		this->bootstrap(this->threadHelper);
	}

	public: template <typename TController, typename TView> std::shared_ptr<TController> createController(std::shared_ptr<TView> view)
	{
		auto controller = std::make_shared<TController>();

		controller->setApplicationObject(this->applicationObject);
		controller->setView(view);

		this->bootstrap(controller);
		this->bootstrap(view);

		return controller;
	}

	public: template <typename T> std::shared_ptr<T> create()
	{
		auto obj = std::make_shared<T>();
		this->bootstrap(obj);
		return obj;
	}

	public: template <typename T> void bootstrap(std::shared_ptr<T> obj)
	{
		{
			std::shared_ptr<Gc::Model::EnvConnection> objc = std::dynamic_pointer_cast<Gc::Model::EnvConnection>(obj);
			if (objc) {assert(this->env); objc->setEnv(this->env);}
		}
		{
			std::shared_ptr<Gc::Model::ListCfg::ListCfgConnection> objc = std::dynamic_pointer_cast<Gc::Model::ListCfg::ListCfgConnection>(obj);
			if (objc) {assert(this->listcfg); objc->setListCfg(this->listcfg);}
		}
		{
			std::shared_ptr<Gc::Model::SettingsManagerDataConnection> objc = std::dynamic_pointer_cast<Gc::Model::SettingsManagerDataConnection>(obj);
			if (objc) {assert(this->settings); objc->setSettingsManager(this->settings);}
		}
		{
			std::shared_ptr<Gc::Model::InstallerConnection> objc = std::dynamic_pointer_cast<Gc::Model::InstallerConnection>(obj);
			if (objc) {assert(this->installer); objc->setInstaller(this->installer);}
		}
		{
			std::shared_ptr<Gc::Model::Device::MountTableConnection> objc = std::dynamic_pointer_cast<Gc::Model::Device::MountTableConnection>(obj);
			if (objc) {assert(this->mountTable); objc->setMountTable(this->mountTable);}
		}
		{
			std::shared_ptr<Gc::Model::FbResolutionsGetterConnection> objc = std::dynamic_pointer_cast<Gc::Model::FbResolutionsGetterConnection>(obj);
			if (objc) {assert(this->fbResolutionsGetter); objc->setFbResolutionsGetter(this->fbResolutionsGetter);}
		}
		{
			std::shared_ptr<Gc::Model::Device::DeviceDataListConnection> objc = std::dynamic_pointer_cast<Gc::Model::Device::DeviceDataListConnection>(obj);
			if (objc) {assert(this->deviceDataList); objc->setDeviceDataList(this->deviceDataList);}
		}
		{
			std::shared_ptr<Gc::Model::Device::DeviceDataListInterfaceConnection> objc = std::dynamic_pointer_cast<Gc::Model::Device::DeviceDataListInterfaceConnection>(obj);
			if (objc) {assert(this->deviceDataList); objc->setDeviceDataList(this->deviceDataList);}
		}
		{
			std::shared_ptr<Gc::Model::ContentParser::GenericFactoryConnection> objc = std::dynamic_pointer_cast<Gc::Model::ContentParser::GenericFactoryConnection>(obj);
			if (objc) {assert(this->contentParserFactory); objc->setContentParserFactory(this->contentParserFactory);}
		}
		{
			std::shared_ptr<Gc::View::Mapper::EntryNameConnection> objc = std::dynamic_pointer_cast<Gc::View::Mapper::EntryNameConnection>(obj);
			if (objc) {assert(this->entryNameMapper); objc->setEntryNameMapper(this->entryNameMapper);}
		}
		{
			std::shared_ptr<Gc::Model::ThemeManagerConnection> objc = std::dynamic_pointer_cast<Gc::Model::ThemeManagerConnection>(obj);
			if (objc) {assert(this->themeManager); objc->setThemeManager(this->themeManager);}
		}
		{
			std::shared_ptr<Gc::Model::Device::DeviceMapConnection> objc = std::dynamic_pointer_cast<Gc::Model::Device::DeviceMapConnection>(obj);
			if (objc) {assert(this->deviceMap); objc->setDeviceMap(this->deviceMap);}
		}
		{
			std::shared_ptr<Gc::Model::Logger::Trait::LoggerAware> objc = std::dynamic_pointer_cast<Gc::Model::Logger::Trait::LoggerAware>(obj);
			if (objc) {assert(this->logger); objc->setLogger(this->logger);}
		}
		{
			std::shared_ptr<Gc::Common::Regex::GenericConnection> objc = std::dynamic_pointer_cast<Gc::Common::Regex::GenericConnection>(obj);
			if (objc) {assert(this->regexEngine); objc->setRegexEngine(this->regexEngine);}
		}
		{
			std::shared_ptr<Gc::Common::Mutex::GenericConnection> objc = std::dynamic_pointer_cast<Gc::Common::Mutex::GenericConnection>(obj);
			if (objc) {objc->setMutex(this->createMutex());}
		}
		{
			std::shared_ptr<Gc::Controller::Helper::ThreadConnection> objc = std::dynamic_pointer_cast<Gc::Controller::Helper::ThreadConnection>(obj);
			if (objc) {assert(this->threadHelper); objc->setThreadHelper(this->threadHelper);}
		}
		{
			std::shared_ptr<Gc::Controller::Helper::RuleMoverConnection> objc = std::dynamic_pointer_cast<Gc::Controller::Helper::RuleMoverConnection>(obj);
			if (objc) {assert(this->ruleMover); objc->setRuleMover(this->ruleMover);}
		}
	}

	// external implementations
	private: std::shared_ptr<Gc::Common::Regex::Generic> createRegexExgine();
	private: std::shared_ptr<Gc::Common::Mutex::Generic> createMutex();
	private: std::shared_ptr<Gc::Controller::Helper::Thread> createThreadHelper();
};}}

#endif /* SRC_BOOTSTRAP_FACTORY_HPP_ */
