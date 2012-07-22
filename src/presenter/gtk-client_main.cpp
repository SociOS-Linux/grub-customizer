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
	GrubConfUIGtk* listCfgView = new GrubConfUIGtk(*listcfg);
	SettingsManagerDataStore* settings = new SettingsManagerDataStore(env);
	SettingsManagerDataStore* settingsOnDisk = new SettingsManagerDataStore(env);
	
	GrubSettingsDlgGtk* settingsDlg = new GrubSettingsDlgGtk(*settings, env);
	presenter.setModelListCfg(*listcfg);
	presenter.setViewListCfg(*listCfgView);
	presenter.setViewSettingsDialog(*settingsDlg);
	presenter.setModelSettingsManager(*settings);
	presenter.setSettingsBuffer(*settingsOnDisk);

	listcfg->connectUI(*listCfgView);
	
	EventListenerView evt_view(presenter);
	listCfgView->setEventListener(evt_view);
	
	EventListenerModel evt_model(presenter);
	listcfg->setEventListener(evt_model);
	
	presenter.run();
}







