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

#ifndef EVENTLISTENER_LISTCFGDLG_INCLUDED
#define EVENTLISTENER_LISTCFGDLG_INCLUDED

/**
 * base class to be implemented by grublistconfig event listeners
 */
class EventListener_listCfgDlg {
public:
	//user wants to show the settings dialog
	virtual void settings_dialog_request()=0;
	//user wants to reload the whole configuration
	virtual void reload_request()=0;
	//user wants to save all his changes
	virtual void save_request()=0;
	//user wants to show the partition chooser
	virtual void rootSelector_request()=0;
	//user wants to show the grub install dialog
	virtual void installDialogRequest()=0;
	//user wants to show the dialog which lets him add new scripts
	virtual void scriptAddDlg_requested()=0;
	//user wants to remove one of the proxies (script instances)
	virtual void removeProxy_requested(void* p)=0;
	//user wants to show the about dialog
	virtual void aboutDialog_requested()=0;
	//user wants to quit -> application asks if it should be closed (return values: true = close, false = stay opened)
	virtual void exitRequest()=0;

	//user has activated or disabled one of the proxies (script instances)
	virtual void signal_script_state_toggled(void* script)=0;
	//user has activated or disabled one of the entries
	virtual void signal_entry_state_toggled(void* entry)=0;
	//user has renamed one of the entries
	virtual void signal_entry_renamed(void* entry)=0;

	//user wants to swap two rules
	virtual void ruleAdjustment_requested(void* rule, int direction)=0;
	//user wants to swap two proxies
	virtual void proxySwap_requested(void* a, void* b)=0;

	//user has selected another rule
	virtual void ruleSelected(void* rule)=0;
	//user has selected another proxy
	virtual void proxySelected(void* proxy)=0;

	//user has used the close button of the burg switcher window
	virtual void burgSwitcher_cancelled()=0;

	//user has chosen burg or grub from the burgSwitcher
	virtual void burgSwitcher_response(bool burgChosen)=0;

	//user has clicked the yes or no button of the partition chooser question dialog
	virtual void partitionChooserQuestion_response(bool is_positive)=0;
};

#endif
