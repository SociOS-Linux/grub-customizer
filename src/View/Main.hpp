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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef GRUBLISTCFGDLG_H_
#define GRUBLISTCFGDLG_H_

#include <string>
#include <vector>
#include <map>
#include <list>

#include <functional>

/**
 * Interface for dialogs which lets the user control the grub list
 */
namespace Gc { namespace View { class Main :
	public Gc::Model::Logger::Trait::LoggerAware
{
	public: virtual inline ~Main() {};

	public: std::function<void (std::list<Gc::Common::Type::Rule*> rules, bool force)> onRemoveRulesClick;
	public: std::function<void ()> onShowSettingsClick;
	public: std::function<void ()> onReloadClick;
	public: std::function<void ()> onSaveClick;
	public: std::function<void ()> onShowEnvEditorClick;
	public: std::function<void ()> onShowInstallerClick;
	public: std::function<void (std::list<Gc::Common::Type::Rule*> childItems)> onCreateSubmenuClick;
	public: std::function<void (std::list<Gc::Common::Type::Rule*> childItems)> onRemoveSubmenuClick;
	public: std::function<void (Gc::Common::Type::Rule* rule)> onShowEntryEditorClick;
	public: std::function<void ()> onShowEntryCreatorClick;
	public: std::function<void ()> onShowAboutClick;
	public: std::function<void ()> onExitClick;
	public: std::function<void (Gc::Common::Type::Rule* entry, std::string const& newText)> onRenameClick;
	public: std::function<void ()> onRevertClick;
	public: std::function<void (std::list<Gc::Common::Type::Rule*> rules, int direction)> onMoveClick;
	public: std::function<void ()> onCancelBurgSwitcherClick;
	public: std::function<void (bool burgChosen)> onInitModeClick;
	public: std::function<void (Gc::Common::Type::Rule* rule, bool startEdit)> onRuleSelection;
	public: std::function<void (unsigned int pos)> onTabChange;
	public: std::function<void (Gc::Common::Type::ViewOption option, bool value)> onViewOptionChange;
	public: std::function<void (Gc::Common::Type::Rule* entry, bool state)> onEntryStateChange;
	public: std::function<void (std::list<Gc::Common::Type::Rule*> selectedRules)> onSelectionChange;


	//show this dialog without waiting
	public: virtual void show() = 0;
	//hide this dialog
	public: virtual void hide() = 0;
	//show this dialog and wait until the window has been closed
	public: virtual void run() = 0;
	//hide this window and close the whole application
	public: virtual void close() = 0;
	//show the dialog which lets the user choose burg or grub
	public: virtual void showBurgSwitcher() = 0;
	//hide the dialog which lets the user choose burg or grub
	public: virtual void hideBurgSwitcher() = 0;
	//returns whether the list configuration window is visible at the moment
	public: virtual bool isVisible() = 0;

	//notifies the window about which mode is used (grub<>burg)
	public: virtual void setIsBurgMode(bool isBurgMode) = 0;
	//determines what users should be able to do and what not
	public: virtual void setLockState(int state) = 0;
	public: virtual void updateLockState() = 0;

	//set the progress of the actual action (loading/saving) to be showed as progress bar for example
	public: virtual void setProgress(double progress) = 0;
	//pulse the progress
	public: virtual void progress_pulse() = 0;
	//hide the progress bar, will be executed after loading has been completed
	public: virtual void hideProgressBar() = 0;
	//sets the text to be showed inside the status bar
	public: virtual void setStatusText(std::string const& new_status_text) = 0;
	public: virtual void setStatusText(std::string const& name, int pos, int max) = 0;
	//add entry to the end of the last script of the list
	public: virtual void appendEntry(Gc::View::Model::ListItem<Gc::Common::Type::Rule, Gc::Common::Type::Proxy> const& listItem) = 0;
	//notifies the user about the problem that no grublistcfg_proxy has been found
	public: virtual void showProxyNotFoundMessage() = 0;
	//creates a string for an other entry placeholder
	public: virtual std::string createNewEntriesPlaceholderString(std::string const& parentMenu) = 0;
	//creates the string for plaintexts
	public: virtual std::string createPlaintextString() const=0;

	//asks the user if he wants to exit the whole application
	public: virtual int showExitConfirmDialog(int type) = 0;
	//show the given error message
	public: virtual void showErrorMessage(std::string const& msg) = 0;

	public: virtual void showConfigSavingError(std::string const& message) = 0;

	//shows an error message including an option for changing the environment
	public: virtual bool askForEnvironmentSettings(std::string const& failedCmd, std::string const& errorMessage) = 0;
	//remove everything from the list
	public: virtual void clear() = 0;

	//asks the user whether the current config should be dropped while another action is started
	public: virtual bool confirmUnsavedSwitch() = 0;

	//assigns a new name to the rule item
	public: virtual void setRuleName(Gc::Common::Type::Rule* rule, std::string const& newName) = 0;

	//select the given rule
	public: virtual void selectRule(Gc::Common::Type::Rule* rule, bool startEdit = false) = 0;

	// select multiple rules
	public: virtual void selectRules(std::list<Gc::Common::Type::Rule*> rules) = 0;

	// set whether the trash pane should be visible
	public: virtual void setTrashPaneVisibility(bool value) = 0;

	// show the warning that config has changed to propose a reload
	public: virtual void showReloadRecommendation() = 0;

	// hide the warning that config has changed to propose a reload
	public: virtual void hideReloadRecommendation() = 0;

	public: virtual void showPlaintextRemoveWarning() = 0;

	public: virtual void showScriptUpdateInfo() = 0;
	public: virtual void hideScriptUpdateInfo() = 0;

	public: virtual void showSystemRuleRemoveWarning() = 0;

	public: virtual void setOption(Gc::Common::Type::ViewOption option, bool value) = 0;

	public: virtual std::map<Gc::Common::Type::ViewOption, bool> const& getOptions() = 0;
	public: virtual void setOptions(std::map<Gc::Common::Type::ViewOption, bool> const& options) = 0;

	public: virtual void setEntryVisibility(Gc::Common::Type::Rule* entry, bool value) = 0;
};}}

#endif
