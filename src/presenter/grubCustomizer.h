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

#ifndef GTK_CLIENT_INCLUDED
#define GTK_CLIENT_INCLUDED

#include "../Model/ListCfg.h"
#include "../View/Main.h"
#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.h"

#include "../Model/Env.h"

#include "../Model/MountTable.h"

#include "../Model/Installer.h"
#include "../View/Installer.h"

#include "../View/Trash.h"
#include "../View/EntryEditor.h"

#include "../Model/ListCfg.h"
#include "../View/Settings.h"
#include "../Model/FbResolutionsGetter.h"
#include "../View/About.h"
#include "../Model/DeviceDataList.h"
#include "../interface/threadController.h"
#include "../interface/contentParserFactory.h"
#include "../View/EnvEditor.h"

#include "commonClass.h"

/**
 * master class of Grub Customizer.
 * Coordinates all the windows (views) and data objects.
 *
 * This application is based on one presenter (this object) and multiple views and models which are
 * controlled by the presenter (MVP). To be independent of the concrete implementation of model and view
 * this class doesn't create any of these objects. They must be set from outside using the set-Methods.
 * This allows to simply change the view class simply by providing other implementations of the given
 * interfaces. The model doesn't use interfaces yet, but it's set from outside too. So it should be
 * not too much work to change this.
 */

class GrubCustomizer : public CommonClass {
	Model_Env& env;
	Model_ListCfg* grublistCfg;
	View_Main* listCfgDlg;
	View_Settings* settingsDlg;
	Model_SettingsManagerData* settings;
	Model_SettingsManagerData* settingsOnDisk; //buffer for the existing settings
	Model_Installer* installer;
	View_Installer* installDlg;
	View_Trash* entryAddDlg;
	View_EntryEditor* entryEditDlg;
	Model_ListCfg* savedListCfg;
	Model_FbResolutionsGetter* fbResolutionsGetter;
	Model_DeviceDataList* deviceDataList;
	Model_MountTable* mountTable;
	View_About* aboutDialog;
	ThreadController* threadController;
	ContentParserFactory* contentParserFactory;
	ContentParser* currentContentParser;
	View_EnvEditor* grubEnvEditor;

	bool config_has_been_different_on_startup_but_unsaved;
	bool modificationsUnsaved;
	bool quit_requested;
	bool is_loading;
	int activeThreadCount;
	Model_ListCfg::Exception thrownException; //to be used from the die() function

	std::string _mapEntryName(Model_Entry const* entry, std::string const& defaultName, std::string const& scriptName = "");
	void _rAppendRule(Model_Rule& rule, Model_Rule* parentRule = NULL);

public:
	enum Exception {
		INCOMPLETE
	};
	void setListCfg(Model_ListCfg& grublistCfg);
	void setListCfgDlg(View_Main& listCfgDlg);
	void setSettingsDialog(View_Settings& settingsDlg);
	void setSettingsManager(Model_SettingsManagerData& settings);
	void setSettingsBuffer(Model_SettingsManagerData& settings);
	void setInstaller(Model_Installer& installer);
	void setInstallDlg(View_Installer& installDlg);
	void setScriptAddDlg(View_Trash& scriptAddDlg);
	void setEntryEditDlg(View_EntryEditor& entryEditDlg);
	void setSavedListCfg(Model_ListCfg& savedListCfg);
	void setFbResolutionsGetter(Model_FbResolutionsGetter& fbResolutionsGetter);
	void setDeviceDataList(Model_DeviceDataList& deviceDataList);
	void setMountTable(Model_MountTable& mountTable);
	void setAboutDialog(View_About& aboutDialog);
	void setThreadController(ThreadController& threadController);
	void setContentParserFactory(ContentParserFactory& contentParserFactory);
	void setGrubEnvEditor(View_EnvEditor& envEditor);

	ThreadController& getThreadController();
	Model_FbResolutionsGetter& getFbResolutionsGetter();

	//init functions
	void init();
	void init(Model_Env::Mode mode, bool initEnv = true);
	void showEnvEditor(bool resetPartitionChooser = false);
	void handleCancelResponse();

	void showSettingsDlg();
	void reload();
	void load(bool preserveConfig = false);
	void save();
	void save_thread();
	GrubCustomizer(Model_Env& env);
	
	void renameEntry(Model_Rule* rule, std::string const& newName);
	void reset();
	
	void showInstallDialog();
	void installGrub(std::string device);
	void showMessageGrubInstallCompleted(std::string const& msg);
	
	void showEntryAddDlg();
	void showEntryEditDlg(void* rule);
	void syncEntryEditDlg(bool useOptionsAsSource);
	void entryEditDlg_buildSource(std::string const& newType);
	void showEntryCreateDlg();
	void addEntryFromEntryAddDlg();
	
	//dispatchers
	void syncListView_load();
	void syncListView_save();
	void die();
	void activateSettingsBtn();
	void updateSettingsDlg();
	void updateList();
	
	void quit(bool force = false);
	
	void removeRules(std::list<void*> entries);
	void updateRuleName(Model_Rule* entry, std::string const& newText);
	void moveRules(std::list<void*> rules, int direction);
	void createSubmenu(std::list<void*> childItems);
	void removeSubmenu(std::list<void*> childItems);
	
	void revertList();

	void showRuleInfo(Model_Rule* rule);
	void showProxyInfo(Model_Proxy* proxy);

	void showAboutDialog();

	void applyEntryEditorModifications();

	//settings dialog
	void updateSettingsDlgResolutionList_dispatched();

	void syncSettings();
	void updateDefaultSetting();
	void updateCustomSetting(std::string const& name);
	void addNewCustomSettingRow();
	void removeCustomSettingRow(std::string const& name);
	void updateShowMenuSetting();
	void updateOsProberSetting();
	void updateTimeoutSetting();
	void updateKernalParams();
	void updateGenerateRecoverySetting();
	void updateCustomResolution();
	void updateColorSettings();
	void updateFontSettings(bool removeFont);
	void updateBackgroundImage();
	void updateUseCustomResolution();
	void removeBackgroundImage();
	void hideSettingsDialog();

	//partition chooser
	void mountSubmountpoint(std::string const& submountpoint);
	void umountSubmountpoint(std::string const& submountpoint);
	void generateSubmountpointSelection(std::string const& prefix);

	// env editor
	void switchPartition(std::string const& newPartition);
	void switchBootloaderType(int newTypeIndex);
	void updateGrubEnvOptions();
	void applyEnvEditor(bool saveConfig);
};

#endif
