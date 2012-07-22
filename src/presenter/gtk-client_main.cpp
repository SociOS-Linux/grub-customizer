#include "gtk-client.h"
#include "eventListener_view.h"
#include "eventListener_model.h"
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
	savedListCfg->verbose = false;
	FbResolutionsGetter* fbResolutionsGetter = new FbResolutionsGetter;

	
	GrubSettingsDlgGtk* settingsDlg = new GrubSettingsDlgGtk(*settings, env);
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
	
	EventListenerView evt_view(presenter);
	listCfgView->setEventListener(evt_view);
	installDlg->setEventListener(evt_view);
	scriptAddDlg->setEventListener(evt_view);
	settingsDlg->setEventListener(evt_view);
	
	EventListenerModel evt_model(presenter);
	listcfg->setEventListener(evt_model);
	installer->setEventListener(evt_model);
	fbResolutionsGetter->setEventListener(evt_model);
	
	presenter.run();
}







