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
#include "../Model/ListCfg.hpp"
#include "../Model/MountTable.hpp"
#include "../Model/SettingsManagerData.hpp"
#include "../Model/Installer.hpp"
#include "../Model/FbResolutionsGetter.hpp"
#include "../Model/DeviceDataList.hpp"
#include "../lib/ContentParser/FactoryImpl.hpp"
#include "../Mapper/EntryNameImpl.hpp"
#include "../Model/ThemeManager.hpp"
#include "../Model/DeviceMap.hpp"

class Bootstrap_Factory
{
	public: std::shared_ptr<Model_Env> env;
	public: std::shared_ptr<Model_ListCfg> listcfg;
	public: std::shared_ptr<Model_SettingsManagerData> settings;
	public: std::shared_ptr<Model_Installer> installer;
	public: std::shared_ptr<Model_MountTable> mountTable;
	public: std::shared_ptr<Model_FbResolutionsGetter> fbResolutionsGetter;
	public: std::shared_ptr<Model_DeviceDataList> deviceDataList;
	public: std::shared_ptr<ContentParser_FactoryImpl> contentParserFactory;
	public: std::shared_ptr<Mapper_EntryNameImpl> entryNameMapper;
	public: std::shared_ptr<Model_ThemeManager> themeManager;
	public: std::shared_ptr<Model_DeviceMap> deviceMap;

	public: std::shared_ptr<Bootstrap_Application_Object> applicationObject;

	public: Bootstrap_Factory(std::shared_ptr<Bootstrap_Application_Object> applicationObject)
	{
		this->applicationObject    = applicationObject;

		this->env                  = std::make_shared<Model_Env>();
		this->listcfg              = std::make_shared<Model_ListCfg>();
		this->settings             = std::make_shared<Model_SettingsManagerData>();
		this->installer            = std::make_shared<Model_Installer>();
		this->mountTable           = std::make_shared<Model_MountTable>();
		this->fbResolutionsGetter  = std::make_shared<Model_FbResolutionsGetter>();
		this->deviceDataList       = std::make_shared<Model_DeviceDataList>();
		this->contentParserFactory = std::make_shared<ContentParser_FactoryImpl>();
		this->entryNameMapper      = std::make_shared<Mapper_EntryNameImpl>();
		this->themeManager         = std::make_shared<Model_ThemeManager>();
		this->deviceMap            = std::make_shared<Model_DeviceMap>();
	}

	public: template <typename TController, typename TView> std::shared_ptr<TController> createController(std::shared_ptr<TView> view)
	{
		auto controller = std::make_shared<TController>();

		controller->setApplicationObject(this->applicationObject);
		controller->setView(view);

		this->bootstrap(controller);

		return controller;
	}

	public: template <typename T> void bootstrap(std::shared_ptr<T> obj)
	{
		{
			std::shared_ptr<Model_Env_Connection> objc = std::dynamic_pointer_cast<Model_Env_Connection>(obj);
			if (objc) {objc->setEnv(this->env);}
		}
		{
			std::shared_ptr<Model_ListCfg_Connection> objc = std::dynamic_pointer_cast<Model_ListCfg_Connection>(obj);
			if (objc) {objc->setListCfg(this->listcfg);}
		}
		{
			std::shared_ptr<Model_SettingsManagerData_Connection> objc = std::dynamic_pointer_cast<Model_SettingsManagerData_Connection>(obj);
			if (objc) {objc->setSettingsManager(this->settings);}
		}
		{
			std::shared_ptr<Model_Installer_Connection> objc = std::dynamic_pointer_cast<Model_Installer_Connection>(obj);
			if (objc) {objc->setInstaller(this->installer);}
		}
		{
			std::shared_ptr<Model_MountTable_Connection> objc = std::dynamic_pointer_cast<Model_MountTable_Connection>(obj);
			if (objc) {objc->setMountTable(this->mountTable);}
		}
		{
			std::shared_ptr<Model_FbResolutionsGetter_Connection> objc = std::dynamic_pointer_cast<Model_FbResolutionsGetter_Connection>(obj);
			if (objc) {objc->setFbResolutionsGetter(this->fbResolutionsGetter);}
		}
		{
			std::shared_ptr<Model_DeviceDataList_Connection> objc = std::dynamic_pointer_cast<Model_DeviceDataList_Connection>(obj);
			if (objc) {objc->setDeviceDataList(this->deviceDataList);}
		}
		{
			std::shared_ptr<Model_DeviceDataListInterface_Connection> objc = std::dynamic_pointer_cast<Model_DeviceDataListInterface_Connection>(obj);
			if (objc) {objc->setDeviceDataList(this->deviceDataList);}
		}
		{
			std::shared_ptr<ContentParserFactory_Connection> objc = std::dynamic_pointer_cast<ContentParserFactory_Connection>(obj);
			if (objc) {objc->setContentParserFactory(this->contentParserFactory);}
		}
		{
			std::shared_ptr<Mapper_EntryName_Connection> objc = std::dynamic_pointer_cast<Mapper_EntryName_Connection>(obj);
			if (objc) {objc->setEntryNameMapper(this->entryNameMapper);}
		}
		{
			std::shared_ptr<Model_ThemeManager_Connection> objc = std::dynamic_pointer_cast<Model_ThemeManager_Connection>(obj);
			if (objc) {objc->setThemeManager(this->themeManager);}
		}
		{
			std::shared_ptr<Model_DeviceMap_Connection> objc = std::dynamic_pointer_cast<Model_DeviceMap_Connection>(obj);
			if (objc) {objc->setDeviceMap(this->deviceMap);}
		}
	}
};

#endif /* SRC_BOOTSTRAP_FACTORY_HPP_ */
