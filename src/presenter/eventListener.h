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

#ifndef EVENTLISTENER_INCLUDED
#define EVENTLISTENER_INCLUDED
#include "grubCustomizer.h"

#include "../interface/evt_grubInstallDlg.h"
#include "../interface/evt_model.h"
#include "../interface/evt_listCfgDlg.h"
#include "../interface/evt_partitionChooser.h"
#include "../interface/evt_scriptAddDlg.h"
#include "../interface/evt_settings.h"

class EventListener :
	public EventListener_settings,
	public EventListener_partitionChooser,
	public EventListener_grubInstallDlg,
	public EventListener_listCfgDlg,
	public EventListener_model,
	public EventListener_scriptAddDlg
{
	GrubCustomizer& presenter;
public:
	EventListener(GrubCustomizer& presenter);
	void settings_dialog_request();
	void reload_request();
	void save_request();
	void rootSelectorCompleted();
	void rootSelector_request();
	void installDialogRequest();
	void installGrub_request(std::string const& device);
	void scriptAddDlg_requested();
	void scriptAddDlg_applied();
	void scriptSelected();
	void removeProxy_requested(void* p);
	void exitRequest();

	void signal_script_state_toggled(void* script);
	void signal_entry_state_toggled(void* entry);
	void signal_entry_renamed(void* entry);

	void ruleAdjustment_requested(void* rule, int direction);
	void proxySwap_requested(void* a, void* b);

	void ruleSelected(void* rule);
	void proxySelected(void* proxy);

	void burgSwitcher_cancelled();
	void burgSwitcher_response(bool burgChosen);
	void partitionChooserQuestion_response(bool is_positive);

	void aboutDialog_requested();

	//settings dialog
	void setting_row_changed(std::string const& name);
	void customRow_insert_requested();
	void customRow_remove_requested(std::string const& name);
	void default_entry_predefined_toggeled();
	void default_entry_saved_toggeled();
	void default_entry_changed();
	void showMenu_toggled();
	void osProber_toggled();
	void timeout_changed();
	void kernelparams_changed();
	void generateRecovery_toggled();
	void useCustomResolution_toggled();
	void resolution_changed();
	void colorChange_requested();
	void backgroundChange_requested();
	void backgroundRemove_requested();
	void settings_dialog_hide_request();

	//root selector
	void partitionChooser_applied();
	void partitionChooser_cancelled();
	void rootFsMount_request();
	void rootFsUmount_request();
	void submountpoint_mount_request(std::string const& mountpoint);
	void submountpoint_umount_request(std::string const& mountpoint);

	//model
	void loadProgressChanged();
	void saveProgressChanged();
	void grubInstallCompleted(std::string const& msg);
	void fb_resolutions_loaded();
};
#endif
