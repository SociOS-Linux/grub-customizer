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

#include "TrashControllerImpl.h"

TrashControllerImpl::TrashControllerImpl(Model_Env& env)
	: ControllerAbstract("trash"),
	  grublistCfg(NULL),
	  view(NULL),
	 env(env),
	 entryNameMapper(NULL)
{
}

void TrashControllerImpl::_refreshView() {
	this->view->clear();

	std::list<Model_Entry*> removedEntries = this->grublistCfg->getRemovedEntries();

	for (std::list<Model_Entry*>::iterator iter = removedEntries.begin(); iter != removedEntries.end(); iter++) {
		Model_Script* script = this->grublistCfg->repository.getScriptByEntry(**iter);
		assert(script != NULL);

		std::string name = (*iter)->name;
		name = this->entryNameMapper->map(&**iter, name, script->name);

		this->view->addItem(name, (*iter)->type != Model_Entry::MENUENTRY, script->name, *iter);
	}

	this->view->setDeleteButtonEnabled(this->_getDeletableEntries().size());
}

std::list<Model_Entry*> TrashControllerImpl::_getDeletableEntries() {
	std::list<Model_Entry*> result;

	std::list<Model_Entry*> removedEntries = this->grublistCfg->getRemovedEntries();
	for (std::list<Model_Entry*>::iterator iter = removedEntries.begin(); iter != removedEntries.end(); iter++) {
		if ((*iter)->type != Model_Entry::MENUENTRY) {
			continue;
		}
		Model_Script* script = this->grublistCfg->repository.getScriptByEntry(**iter);
		assert(script != NULL);
		if (script->isCustomScript) {
			result.push_back(*iter);
		}
	}

	return result;
}

void TrashControllerImpl::setListCfg(Model_ListCfg& grublistCfg){
	this->grublistCfg = &grublistCfg;
}

void TrashControllerImpl::setView(View_Trash& scriptAddDlg){
	this->view = &scriptAddDlg;
}

void TrashControllerImpl::setEntryNameMapper(Mapper_EntryName& mapper) {
	this->entryNameMapper = &mapper;
}

void TrashControllerImpl::showAction(){
	this->logActionBegin("show");
	try {
		this->_refreshView();

		view->show();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void TrashControllerImpl::applyAction(){
	this->logActionBegin("apply");
	try {
		std::list<Entry*> entries = view->getSelectedEntries();
		this->getAllControllers().mainController->addEntriesAction(entries);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}


void TrashControllerImpl::hideAction() {
	this->logActionBegin("hide");
	try {
		this->view->hide();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void TrashControllerImpl::askForDeletionAction() {
	this->logActionBegin("ask-for-deletion");
	try {
		std::list<Model_Entry*> deletableEntries = this->_getDeletableEntries();
		std::list<std::string> deletableEntryNames;
		for (std::list<Model_Entry*>::iterator iter = deletableEntries.begin(); iter != deletableEntries.end(); iter++) {
			Model_Script* script = this->grublistCfg->repository.getScriptByEntry(**iter);
			assert(script != NULL);
			deletableEntryNames.push_back(this->entryNameMapper->map(&**iter, (*iter)->name, script->name));
		}
		this->view->askForDeletion(deletableEntryNames);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void TrashControllerImpl::deleteCustomEntriesAction() {
	this->logActionBegin("delete-custom-entries");
	try {
		std::list<Model_Entry*> deletableEntries = this->_getDeletableEntries();
		for (std::list<Model_Entry*>::iterator iter = deletableEntries.begin(); iter != deletableEntries.end(); iter++) {
			this->grublistCfg->deleteEntry(**iter);
		}
		this->_refreshView();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}
