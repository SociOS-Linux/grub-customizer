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

#include "TrashControllerImpl.h"

TrashControllerImpl::TrashControllerImpl(Model_Env& env)
	: grublistCfg(NULL),
	  view(NULL),
	 env(env),
	 entryNameMapper(NULL)
{
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
	view->clear();

	std::list<Model_Entry*> removedEntries = this->grublistCfg->getRemovedEntries();
	for (std::list<Model_Entry*>::iterator iter = removedEntries.begin(); iter != removedEntries.end(); iter++) {
		Model_Script* script = this->grublistCfg->repository.getScriptByEntry(**iter);

		std::string name = (*iter)->name;
		name = this->entryNameMapper->map(&**iter, name, script->name);

		view->addItem(name, (*iter)->type != Model_Entry::MENUENTRY, script->name, *iter);
	}

	view->show();
}

void TrashControllerImpl::applyAction(){
	std::list<void*> entries = view->getSelectedEntries();
	this->getAllControllers().mainController->addEntriesAction(entries);
}


void TrashControllerImpl::hideAction() {
	this->view->hide();
}
