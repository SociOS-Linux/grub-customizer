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

#ifndef GRUBLISTCFGDLG_H_
#define GRUBLISTCFGDLG_H_

#include "evt_listCfgDlg.h"
#include <string>
#include <vector>

/**
 * Interface for dialogs which lets the user control the grub list
 */
class GrublistCfgDlg {
public:
	enum Exception {
		PROXY_ITER_NOT_FOUND,
		RULE_ITER_NOT_FOUND
	};
	//function to assign the event listener
	virtual void setEventListener(EventListener_listCfgDlg& eventListener)=0;
	//show this dialog without waiting
	virtual void show()=0;
	//show this dialog and wait until the window has been closed
	virtual void run()=0;
	//hide this window and close the whole application
	virtual void close()=0;
	//show the dialog which lets the user choose burg or grub
	virtual void showBurgSwitcher()=0;
	//hide the dialog which lets the user choose burg or grub
	virtual void hideBurgSwitcher()=0;
	//show the dialog which asks the user if he wants to show the partition chooser
	virtual void showPartitionChooserQuestion()=0;
	//hide the dialog which asks the user if he wants to show the partition chooser
	virtual void hidePartitionChooserQuestion()=0;
	//returns whether the list configuration window is visible at the moment
	virtual bool isVisible()=0;

	//notifies the window about which mode is used (grub<>burg)
	virtual void setIsBurgMode(bool isBurgMode)=0;
	//determines what users should be able to do and what not
	virtual void setLockState(int state)=0;

	//set the progress of the actual action (loading/saving) to be showed as progress bar for example
	virtual void setProgress(double progress)=0;
	//pulse the progress
	virtual void progress_pulse()=0;
	//hide the progress bar, will be executed after loading has been completed
	virtual void hideProgressBar()=0;
	//sets the text to be showed inside the status bar
	virtual void setStatusText(std::string const& new_status_text)=0;
	//add entry to the end of the last script of the list
	virtual void appendEntry(std::string const& name, bool is_active, void* entryPtr, bool editable, bool is_submenu, void* parentEntry = NULL)=0;
	//notifies the user about the problem that no grublistcfg_proxy has been found
	virtual void showProxyNotFoundMessage()=0;
	//creates a string for an other entry placeholder
	virtual std::string createNewEntriesPlaceholderString(std::string const& parentMenu = "", std::string const& sourceScriptName = "")=0;
	//creates the string for plaintexts
	virtual std::string createPlaintextString() const=0;

	//sets the given title to be showed as default title inside the status bar
	virtual void setDefaultTitleStatusText(std::string const& str)=0;

	//asks the user if he wants to exit the whole application
	virtual int showExitConfirmDialog(int type)=0;
	//show the given error message
	virtual void showErrorMessage(std::string const& msg, std::vector<std::string> const& values = std::vector<std::string>())=0;
	//remove everything from the list
	virtual void clear()=0;

	//reads the name of a rule item
	virtual std::string getRuleName(void* rule)=0;
	//assigns a new name to the rule item
	virtual void setRuleName(void* rule, std::string const& newName)=0;
	//reads whether the given rule is activated
	virtual bool getRuleState(void* rule)=0;
	//set whether the given rule is activated
	virtual void setRuleState(void* rule, bool newState)=0;

	//select the given rule
	virtual void selectRule(void* rule, bool startEdit = false)=0;
};

#endif
