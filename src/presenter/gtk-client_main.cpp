#include "gtk-client.h"
#include "eventListener.h"
#include "../model/grubEnv.h"

int main(int argc, char** argv){
	setlocale( LC_ALL, "");
	bindtextdomain( "grub-customizer", LOCALEDIR);
	textdomain( "grub-customizer" );

	Gtk::Main app(argc, argv);
	Glib::thread_init();

	GrubEnv env;
	
	GtkClient presenter(env);

	GrublistCfg* listcfg = new GrublistCfg(env);
	GrublistCfgDlg* listCfgView = new GrublistCfgDlg;
	SettingsManagerDataStore* settings = new SettingsManagerDataStore(env);
	SettingsManagerDataStore* settingsOnDisk = new SettingsManagerDataStore(env);
	GrubInstaller* installer = new GrubInstaller(env);
	GrubInstallDlg* installDlg = new GrubInstallDlg;
	ScriptAddDlg* scriptAddDlg = new ScriptAddDlg;
	PartitionChooser* partitionChooser = new PartitionChooser(GrubEnv::isLiveCD());
	GrublistCfg* savedListCfg = new GrublistCfg(env);
	FbResolutionsGetter* fbResolutionsGetter = new FbResolutionsGetter;
	GrubSettingsDlgGtk* settingsDlg = new GrubSettingsDlgGtk;
	DeviceDataList* deviceDataList = new DeviceDataList;
	MountTable* mountTable = new MountTable;
	AboutDialog* aboutDialog = new AboutDialog;

	
	presenter.setModelListCfg(*listcfg);
	presenter.setViewListCfg(*listCfgView);
	presenter.setViewSettingsDialog(*settingsDlg);
	presenter.setModelSettingsManager(*settings);
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







