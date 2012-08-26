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
#include "../Model/FbResolutionsGetter.h"
#include "../Model/DeviceDataList.h"
#include "ThreadController.h"
#include "../interface/contentParserFactory.h"
#include "../Mapper/EntryName.h"

#include "../Controller/ControllerAbstract.h"

#include "../lib/CommonClass.h"

#include "SettingsController.h"


class SettingsControllerImpl : public ControllerAbstract, public SettingsController {
	Model_Env& env;
	Model_ListCfg* grublistCfg;
	View_Settings* view;
	Model_SettingsManagerData* settings;
	Model_SettingsManagerData* settingsOnDisk; //buffer for the existing settings
	Model_FbResolutionsGetter* fbResolutionsGetter;
	ThreadController* threadController;

public:
	enum Exception {
		INCOMPLETE
	};
	void setListCfg(Model_ListCfg& grublistCfg);
	void setSettingsDialog(View_Settings& settingsDlg);
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
	void updateColorSettingsAction();
	void updateFontSettingsAction(bool removeFont);
	void updateBackgroundImageAction();
	void updateUseCustomResolutionAction();
	void removeBackgroundImageAction();
	void hideAction();
	void showAction(bool burgMode);
	void syncAction();
};

#endif
