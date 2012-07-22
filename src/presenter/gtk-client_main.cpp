#include "grubCustomizer.h"
#include "eventListener.h"
#include "../model/grubEnv.h"
#include "../view/grublistCfgDlgGtk.h"
#include "../view/aboutDialogGtk.h"
#include "../view/grubInstallDlgGtk.h"
#include "../view/partitionChooserGtk.h"
#include "../view/scriptAddDlgGtk.h"
#include "../view/settingsDlgGtk.h"

int main(int argc, char** argv){
	setlocale( LC_ALL, "");
	bindtextdomain( "grub-customizer", LOCALEDIR);
	textdomain( "grub-customizer" );

	Gtk::Main app(argc, argv);
	Glib::thread_init();

	GrubEnv env;
	
	GrubCustomizer presenter(env);

	GrublistCfg* listcfg = new GrublistCfg(env);
	GrublistCfgDlg* listCfgView = new GrublistCfgDlgGtk;
	SettingsManagerDataStore* settings = new SettingsManagerDataStore(env);
	SettingsManagerDataStore* settingsOnDisk = new SettingsManagerDataStore(env);
	GrubInstaller* installer = new GrubInstaller(env);
	GrubInstallDlg* installDlg = new GrubInstallDlgGtk;
	ScriptAddDlg* scriptAddDlg = new ScriptAddDlgGtk;
	PartitionChooser* partitionChooser = new PartitionChooserGtk(GrubEnv::isLiveCD());
	GrublistCfg* savedListCfg = new GrublistCfg(env);
	FbResolutionsGetter* fbResolutionsGetter = new FbResolutionsGetter;
	SettingsDlg* settingsDlg = new GrubSettingsDlgGtk;
	DeviceDataList* deviceDataList = new DeviceDataList;
	MountTable* mountTable = new MountTable;
	AboutDialog* aboutDialog = new AboutDialogGtk;

	
	presenter.setListCfg(*listcfg);
	presenter.setListCfgDlg(*listCfgView);
	presenter.setSettingsDialog(*settingsDlg);
	presenter.setSettingsManager(*settings);
	presenter.setSettingsBuffer(*settingsOnDisk);
	presenter.setInstaller(*installer);
	presenter.setInstallDlg(*installDlg);
	presenter.setScriptAddDlg(*scriptAddDlg);
	presenter.setPartitionChooser(*partitionChooser);
	presenter.setSavedListCfg(*savedListCfg);
	presenter.setFbResolutionsGetter(*fbResolutionsGetter);
	presenter.setDeviceDataList(*deviceDataList);
	presenter.setMountTable(*mountTable);
	presenter.setAboutDialog(*aboutDialog);
	
	EventListener evt(presenter);
	listCfgView->setEventListener(evt);
	installDlg->setEventListener(evt);
	scriptAddDlg->setEventListener(evt);
	settingsDlg->setEventListener(evt);
	partitionChooser->setEventListener(evt);
	
	listcfg->setEventListener(evt);
	installer->setEventListener(evt);
	fbResolutionsGetter->setEventListener(evt);
	
	presenter.run();
}







