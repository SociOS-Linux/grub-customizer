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

#ifndef SETTINGSCONTROLLERIMPL_H_
#define SETTINGSCONTROLLERIMPL_H_

#include "../Model/ListCfg.h"
#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.h"

#include "../Model/Env.h"

#include "../Model/MountTable.h"

#include "../Model/Installer.h"

#include "../Model/ListCfg.h"
#include "../View/Settings.h"
#include "../View/Trait/ViewAware.h"
#include "../Model/FbResolutionsGetter.h"
#include "../Model/DeviceDataList.h"
#include "ThreadController.h"
#include "../lib/ContentParserFactory.h"
#include "../Mapper/EntryName.h"

#include "../Controller/ControllerAbstract.h"

#include "../lib/CommonClass.h"

#include "../lib/Exception.h"

#include "SettingsController.h"


class SettingsControllerImpl : public ControllerAbstract, public SettingsController, public View_Trait_ViewAware<View_Settings> {
	Model_Env& env;
	Model_ListCfg* grublistCfg;
	Model_SettingsManagerData* settings;
	Model_SettingsManagerData* settingsOnDisk; //buffer for the existing settings
	Model_FbResolutionsGetter* fbResolutionsGetter;
	ThreadController* threadController;
	bool syncActive; // should only be controlled by syncSettings()

public:
	void setListCfg(Model_ListCfg& grublistCfg);
	void setSettingsManager(Model_SettingsManagerData& settings);
	void setSettingsBuffer(Model_SettingsManagerData& settings);
	void setFbResolutionsGetter(Model_FbResolutionsGetter& fbResolutionsGetter);
	void setThreadController(ThreadController& threadController);

	ThreadController& getThreadController();
	Model_FbResolutionsGetter& getFbResolutionsGetter();

	void showSettingsDlg();
	SettingsControllerImpl(Model_Env& env);

	//dispatchers
	void updateSettingsDataAction();

	void loadResolutionsAction();

	//settings dialog
	void updateResolutionlistAction();
	void updateResolutionlistThreadedAction();

	void syncSettings();

	void updateDefaultSystemAction();
	void updateCustomSettingAction(std::string const& name);
	void addCustomSettingAction();
	void removeCustomSettingAction(std::string const& name);
	void updateShowMenuSettingAction();
	void updateOsProberSettingAction();
	void updateTimeoutSettingAction();
	void updateKernelParamsAction();
	void updateRecoverySettingAction();
	void updateCustomResolutionAction();
	void updateUseCustomResolutionAction();
	void hideAction();
	void showAction(bool burgMode);
	void syncAction();
};

#endif
