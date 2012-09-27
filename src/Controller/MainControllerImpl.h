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

#ifndef MAINCONNTROLLER_INCLUDED
#define MAINCONNTROLLER_INCLUDED

#include "../Model/ListCfg.h"
#include "../View/Main.h"
#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.h"

#include "../Model/Env.h"

#include "../Model/ListCfg.h"
#include "../Model/FbResolutionsGetter.h"
#include "../Model/DeviceDataList.h"
#include "ThreadController.h"
#include "../lib/ContentParserFactory.h"

#include "../Controller/ControllerAbstract.h"

#include "../lib/CommonClass.h"
#include "../Mapper/EntryName.h"

#include "../lib/Exception.h"

#include "MainController.h"

/**
 * This controller operates on the entry list
 */

class MainControllerImpl : public ControllerAbstract, public MainController {
	Model_Env& env;
	Model_ListCfg* grublistCfg;
	View_Main* view;
	Model_SettingsManagerData* settings;
	Model_SettingsManagerData* settingsOnDisk; //buffer for the existing settings
	Model_ListCfg* savedListCfg;
	Model_FbResolutionsGetter* fbResolutionsGetter;
	Model_DeviceDataList* deviceDataList;
	Model_MountTable* mountTable;
	ThreadController* threadController;
	ContentParserFactory* contentParserFactory;
	ContentParser* currentContentParser;
	Mapper_EntryName* entryNameMapper;

	bool config_has_been_different_on_startup_but_unsaved;
	bool is_loading;
	CmdExecException thrownException; //to be used from the die() function

	void _rAppendRule(Model_Rule& rule, Model_Rule* parentRule = NULL);
	bool _listHasPlaintextRules(std::list<void*> const& rules);
	bool _listHasCurrentSystemRules(std::list<void*> const& rules);

public:
	void setListCfg(Model_ListCfg& grublistCfg);
	void setView(View_Main& listCfgDlg);
	void setSettingsDialog(View_Settings& settingsDlg);
	void setSettingsManager(Model_SettingsManagerData& settings);
	void setSettingsBuffer(Model_SettingsManagerData& settings);
	void setSavedListCfg(Model_ListCfg& savedListCfg);
	void setFbResolutionsGetter(Model_FbResolutionsGetter& fbResolutionsGetter);
	void setDeviceDataList(Model_DeviceDataList& deviceDataList);
	void setMountTable(Model_MountTable& mountTable);
	void setThreadController(ThreadController& threadController);
	void setContentParserFactory(ContentParserFactory& contentParserFactory);
	void setEntryNameMapper(Mapper_EntryName& mapper);

	ThreadController& getThreadController();
	Model_FbResolutionsGetter& getFbResolutionsGetter();

	//init functions
	void init();
	void init(Model_Env::Mode mode, bool initEnv = true);
	void initAction();
	void reInitAction(bool burgMode);
	void showEnvEditorAction(bool resetPartitionChooser = false);
	void cancelBurgSwitcherAction();

	void reloadAction();
	void loadThreadedAction(bool preserveConfig = false);
	void saveAction();
	void saveThreadedAction();
	MainControllerImpl(Model_Env& env);
	
	void renameEntry(Model_Rule* rule, std::string const& newName);
	void reset();
	
	void showInstallerAction();
	
	void showEntryEditorAction(void* rule);
	void showEntryCreatorAction();
	
	//dispatchers
	void dieAction();
	void updateList();
	
	void exitAction(bool force = false);
	
	void removeRulesAction(std::list<void*> rules, bool force = false);
	void renameRuleAction(void* entry, std::string const& newText);
	void moveAction(std::list<void*> rules, int direction);
	void createSubmenuAction(std::list<void*> childItems);
	void removeSubmenuAction(std::list<void*> childItems);
	
	void revertAction();

	void showProxyInfo(Model_Proxy* proxy);

	void showAboutAction();

	void syncLoadStateThreadedAction();
	void syncSaveStateThreadedAction();

	void syncSaveStateAction();
	void syncLoadStateAction();

	void showSettingsAction();
	void showTrashAction();
	void initModeAction(bool burgChosen);
	void addEntriesAction(std::list<void*> entries);
	void activateSettingsAction();
	void showReloadRecommendationAction();
	void selectRulesAction(std::list<void*> rules);
	void selectRuleAction(void* rule, bool startEdit = false);
	void refreshTabAction(unsigned int pos);
	void setViewOptionAction(ViewOption option, bool value);
	void entryStateToggledAction(void* entry, bool state);
};

#endif
