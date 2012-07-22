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
	MountTable* mountTable = new MountTable;
	PartitionChooser* partitionChooser = new PartitionChooserGtk();
	GrublistCfg* savedListCfg = new GrublistCfg(env);
	FbResolutionsGetter* fbResolutionsGetter = new FbResolutionsGetter;
	SettingsDlg* settingsDlg = new GrubSettingsDlgGtk;
	DeviceDataList* deviceDataList = new DeviceDataList;
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
	
	presenter.init();
	app.run();
}







