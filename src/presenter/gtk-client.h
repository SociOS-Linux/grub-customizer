#ifndef GTK_CLIENT_INCLUDED
#define GTK_CLIENT_INCLUDED

#include "../model/grublistCfg.h"
#include "../view/grubconf_ui_gtk.h"
#include <glibmm/thread.h>
#include <glibmm/dispatcher.h>
#include <libintl.h>
#include <locale.h>
#include "../config.h"

#include "../model/grubEnv.h"

#include "../model/mountTable.h"
#include "../view/partitionChooser.h"

#include "../model/grubInstaller.h"
#include "../view/grubInstallDlg.h"

#include "../view/scriptAddDlg.h"

#include "../model/grublistCfg.h"
#include "../view/partitionChooser.h"
#include "../view/settings_dlg_gtk.h"

class GtkClient {
	GrubEnv& env;
	GrublistCfg* grublistCfg;
	GrubConfUIGtk* listCfgDlg;
	GrubSettingsDlgGtk* settingsDlg;
	SettingsManagerDataStore* settings;
	SettingsManagerDataStore* settingsOnDisk; //buffer for the existing settings
	GrubInstaller* installer;
	GrubInstallDlg* installDlg;
	ScriptAddDlg* scriptAddDlg;
	PartitionChooser* partitionChooser;
	
	Glib::Dispatcher disp_sync_load, disp_sync_save, disp_thread_died;
public:
	void setModelListCfg(GrublistCfg& grublistCfg);
	void setViewListCfg(GrubConfUIGtk& listCfgDlg);
	void setViewSettingsDialog(GrubSettingsDlgGtk& settingsDlg);
	void setModelSettingsManager(SettingsManagerDataStore& settings);
	void setSettingsBuffer(SettingsManagerDataStore& settings);
	void setInstaller(GrubInstaller& installer);
	void setInstallDlg(GrubInstallDlg& installDlg);
	void setScriptAddDlg(ScriptAddDlg& scriptAddDlg);
	void setPartitionChooser(PartitionChooser& partitionChooser);

	void showSettingsDlg();
	void load(bool keepConfig = false);
	void save();
	void save_thread();
	GtkClient(GrubEnv& env);
	
	void run();
	void renameEntry(Rule* rule, std::string const& newName);
	void reset();
	bool prepare(bool forceRootSelection = false);
	void startRootSelector();
	
	void syncEntryList();
	void updateSaveProgress();
	void showErrorThreadDied();
	
	void showInstallDialog();
	void installGrub(std::string const& device);
	void showMessageGrubInstallCompleted(std::string const& msg);
	
	void showScriptAddDlg();
	void addScriptFromScriptAddDlg();
	void updateScriptAddDlgPreview();
	
	void removeProxy(Proxy* p);
	
	void syncListView_load();
	void syncListView_save();
	void thread_died_handler();
	
	bool quit();
	
	void syncProxyState(void* proxy);
	void syncRuleState(Rule* entry);
	void syncRuleName(Rule* entry);
	void updateScriptEntry(Proxy* proxy);
	void swapRules(Rule* a, Rule* b);
	void swapProxies(Proxy* a, Proxy* b);
	
	void showRuleInfo(Rule* rule);
	void showProxyInfo(Proxy* proxy);
};

#endif
