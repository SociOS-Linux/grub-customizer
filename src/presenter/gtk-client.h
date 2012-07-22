#ifndef GTK_CLIENT_INCLUDED
#define GTK_CLIENT_INCLUDED

#include "../model/grublistCfg.h"
#include "../view/grubconf_ui_gtk.h"
#include <glibmm/thread.h>
#include <libintl.h>
#include <locale.h>
#include "../config.h"

#include "../model/grubEnv.h"

#include "../model/mountTable.h"
#include "../view/partitionChooser.h"

#include "../model/grubInstaller.h"
#include "../view/grubInstallDlg.h"

class GtkClient {
	GrubEnv& env;
	GrublistCfg* grublistCfg;
	GrubConfUIGtk* listCfgDlg;
	GrubSettingsDlgGtk* settingsDlg;
	SettingsManagerDataStore* settings;
	SettingsManagerDataStore* settingsOnDisk; //buffer for the existing settings
	GrubInstaller* installer;
	GrubInstallDlg* installDlg;
public:
	void setModelListCfg(GrublistCfg& grublistCfg);
	void setViewListCfg(GrubConfUIGtk& listCfgDlg);
	void setViewSettingsDialog(GrubSettingsDlgGtk& settingsDlg);
	void setModelSettingsManager(SettingsManagerDataStore& settings);
	void setSettingsBuffer(SettingsManagerDataStore& settings);
	void setInstaller(GrubInstaller& installer);
	void setInstallDlg(GrubInstallDlg& installDlg);

	void showSettingsDlg();
	void load(bool keepConfig = false);
	void save();
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
};

#endif
