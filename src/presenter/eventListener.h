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
#include "../interface/evt_entryAddDlg.h"
#include "../interface/evt_settings.h"
#include "../interface/evt_entryEditDlg.h"
#include "../interface/evt_grubEnvEditor.h"

class EventListener :
	public EventListener_settings,
	public EventListener_partitionChooser,
	public EventListener_grubInstallDlg,
	public EventListener_listCfgDlg,
	public EventListener_model,
	public EventListener_entryAddDlg,
	public EventListener_entryEditDlg,
	public EventListener_grubEnvEditor
{
	GrubCustomizer& presenter;
public:
	EventListener(GrubCustomizer& presenter);
	void settings_dialog_request();
	void reload_request();
	void save_request();
	void rootSelectorCompleted();
	void rootSelector_request();
	void createSubmenuRequest(std::list<void*> childItems);
	void removeSubmenuRequest(std::list<void*> childItems);
	void installDialogRequest();
	void installGrub_request(std::string const& device);
	void entryAddDlg_requested();
	void entryEditDlg_requested(void* rule);
	void entryAddDlg_applied();
	void entryEditDlg_applied();
	void entryEditDlg_sourceModified();
	void entryEditDlg_optionsModified();
	void exitRequest();

	void signal_entry_remove_requested(std::list<void*> entries);
	void signal_entry_renamed(void* entry, std::string const& newText);

	void ruleAdjustment_requested(std::list<void*> rules, int direction);

	void ruleSelected(void* rule);
	void proxySelected(void* proxy);

	void burgSwitcher_cancelled();
	void burgSwitcher_response(bool burgChosen);

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

	// env editor
	void grubEnvEditor_partitionChanged(std::string const& newPartition);
	void grubEnvEditor_typeChanged(int newTypeIndex);
	void grubEnvEditor_optionModified();
	void grubEnvEditor_cancellationRequested();
};
#endif
