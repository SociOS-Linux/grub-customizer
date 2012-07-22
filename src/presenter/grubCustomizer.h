#ifndef GTK_CLIENT_INCLUDED
#define GTK_CLIENT_INCLUDED

#include "../model/grublistCfg.h"
#include "../interface/grublistCfgDlg.h"
#include <glibmm/thread.h>
#include <glibmm/dispatcher.h>
#include <libintl.h>
#include <locale.h>
#include "../config.h"

#include "../model/grubEnv.h"

#include "../model/mountTable.h"
#include "../interface/partitionChooser.h"

#include "../model/grubInstaller.h"
#include "../interface/grubInstallDlg.h"

#include "../interface/scriptAddDlg.h"

#include "../model/grublistCfg.h"
#include "../interface/partitionChooser.h"
#include "../interface/settingsDlg.h"
#include "../model/fbResolutionsGetter.h"
#include "../interface/aboutDialog.h"
#include "../model/deviceDataList.h"

#include <giomm/file.h>

//TODO: allow settings manager startup after loading the settings - don't wait until everything is loaded

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
class GrubCustomizer {
	GrubEnv& env;
	GrublistCfg* grublistCfg;
	GrublistCfgDlg* listCfgDlg;
	SettingsDlg* settingsDlg;
	SettingsManagerDataStore* settings;
	SettingsManagerDataStore* settingsOnDisk; //buffer for the existing settings
	GrubInstaller* installer;
	GrubInstallDlg* installDlg;
	ScriptAddDlg* scriptAddDlg;
	PartitionChooser* partitionChooser;
	GrublistCfg* savedListCfg;
	FbResolutionsGetter* fbResolutionsGetter;
	DeviceDataList* deviceDataList;
	MountTable* mountTable;
	AboutDialog* aboutDialog;
	
	Glib::Dispatcher disp_sync_load, disp_sync_save, disp_thread_died, disp_updateSettingsDlgResolutionList, disp_settings_loaded;
	bool config_has_been_different_on_startup_but_unsaved;
	bool modificationsUnsaved;
	bool quit_requested;
	int activeThreadCount;
	GrublistCfg::Exception thrownException; //to be used from the die() function
public:
	enum Exception {
		INCOMPLETE
	};
	void setListCfg(GrublistCfg& grublistCfg);
	void setListCfgDlg(GrublistCfgDlg& listCfgDlg);
	void setSettingsDialog(SettingsDlg& settingsDlg);
	void setSettingsManager(SettingsManagerDataStore& settings);
	void setSettingsBuffer(SettingsManagerDataStore& settings);
	void setInstaller(GrubInstaller& installer);
	void setInstallDlg(GrubInstallDlg& installDlg);
	void setScriptAddDlg(ScriptAddDlg& scriptAddDlg);
	void setPartitionChooser(PartitionChooser& partitionChooser);
	void setSavedListCfg(GrublistCfg& savedListCfg);
	void setFbResolutionsGetter(FbResolutionsGetter& fbResolutionsGetter);
	void setDeviceDataList(DeviceDataList& deviceDataList);
	void setMountTable(MountTable& mountTable);
	void setAboutDialog(AboutDialog& aboutDialog);

	void showSettingsDlg();
	void load(bool preserveConfig = false);
	void save();
	void save_thread();
	GrubCustomizer(GrubEnv& env);
	
	void run();
	void renameEntry(Rule* rule, std::string const& newName);
	void reset();
	bool prepare(bool forceRootSelection = false);
	void startRootSelector();
	void initRootSelector();
	
	void syncEntryList();
	void updateSaveProgress();
	void showErrorThreadDied();
	
	void showInstallDialog();
	void installGrub(std::string device);
	void showMessageGrubInstallCompleted(std::string const& msg);
	
	void showScriptAddDlg();
	void addScriptFromScriptAddDlg();
	void updateScriptAddDlgPreview();
	
	void removeProxy(Proxy* p);
	
	//dispatchers
	void syncListView_load();
	void syncListView_save();
	void die();
	void activateSettingsBtn();
	void updateSettingsDlg();
	
	bool quit();
	
	void syncProxyState(void* proxy);
	void syncRuleState(Rule* entry);
	void syncRuleName(Rule* entry);
	void updateScriptEntry(Proxy* proxy);
	void swapRules(Rule* a, Rule* b);
	void swapProxies(Proxy* a, Proxy* b);
	
	void showRuleInfo(Rule* rule);
	void showProxyInfo(Proxy* proxy);

	void showAboutDialog();

	//settings dialog
	void updateSettingsDlgResolutionList();
	void updateSettingsDlgResolutionList_dispatched();

	void syncSettings();
	void updateDefaultSetting();
	void updateCustomSetting(std::string const& name);
	void updateShowMenuSetting();
	void updateOsProberSetting();
	void updateTimeoutSetting();
	void updateKernalParams();
	void updateGenerateRecoverySetting();
	void updateCustomResolution();
	void updateColorSettings();
	void updateBackgroundImage();
	void updateUseCustomResolution();
	void copyBackgroundImageToGrubDirectory();
	void removeBackgroundImage();
	void hideSettingsDialog();

	//partition chooser
	void mountRootFs();
	void umountRootFs();
	void cancelPartitionChooser();
	void applyPartitionChooser();
	void mountSubmountpoint(Glib::ustring const& submountpoint);
	void umountSubmountpoint(Glib::ustring const& submountpoint);
	void readPartitionInfo();
	void generateSubmountpointSelection(std::string const& prefix);
};

#endif
