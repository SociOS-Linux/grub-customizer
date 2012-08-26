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

#ifndef MAINCONTROLLER_H_
#define MAINCONTROLLER_H_

#include <list>
#include "../Controller/ThreadController.h"

class MainController {
public:
	virtual inline ~MainController(){};

	virtual void syncLoadStateThreadedAction() = 0;
	virtual void syncSaveStateThreadedAction() = 0;

	virtual void syncLoadStateAction() = 0;
	virtual void syncSaveStateAction() = 0;

	virtual void reInitAction(bool burgMode) = 0;
	virtual void showSettingsAction() = 0;
	virtual void reloadAction() = 0;
	virtual void saveAction() = 0;
	virtual void showEnvEditorAction(bool resetPartitionChooser = false) = 0;
	virtual void showInstallerAction() = 0;
	virtual void createSubmenuAction(std::list<void*> childItems) = 0;
	virtual void removeSubmenuAction(std::list<void*> childItems) = 0;
	virtual void showTrashAction() = 0;
	virtual void showEntryEditorAction(void* rule) = 0;
	virtual void showEntryCreatorAction() = 0;
	virtual void showAboutAction() = 0;
	virtual void dieAction() = 0;
	virtual void exitAction(bool force = false) = 0;
	virtual void removeRulesAction(std::list<void*> entries) = 0;
	virtual void renameRuleAction(void* entry, std::string const& newText) = 0;
	virtual void revertAction() = 0;
	virtual void moveAction(std::list<void*> rules, int direction) = 0;
	virtual void showInfoAction(void* rule) = 0;
	virtual void cancelBurgSwitcherAction() = 0;
	virtual void initModeAction(bool burgChosen) = 0;
	virtual void loadThreadedAction(bool preserveConfig = false) = 0;
	virtual void saveThreadedAction() = 0;
	virtual void addEntriesAction(std::list<void*> entries) = 0;
	virtual void activateSettingsAction() = 0;
	virtual void showReloadRecommendationAction() = 0;
	virtual void selectRulesAction(std::list<void*> rules) = 0;
	virtual void selectRuleAction(void* rule, bool startEdit = false) = 0;

	virtual ThreadController& getThreadController() = 0;
};


#endif /* MAINCONTROLLER_H_ */
