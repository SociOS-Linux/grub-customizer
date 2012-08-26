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

GrubCustomizer::GrubCustomizer(Model_Env& env)
	: grublistCfg(NULL),
	  installer(NULL), installDlg(NULL),
	  mountTable(NULL), aboutDialog(NULL),
	 env(env),
	 threadController(NULL),
	 entryNameMapper(NULL)
{
}


void GrubCustomizer::setListCfg(Model_ListCfg& grublistCfg){
	this->grublistCfg = &grublistCfg;
}

void GrubCustomizer::setInstaller(Model_Installer& installer){
	this->installer = &installer;
}
void GrubCustomizer::setInstallDlg(View_Installer& installDlg){
	this->installDlg = &installDlg;
}

void GrubCustomizer::setMountTable(Model_MountTable& mountTable){
	this->mountTable = &mountTable;
}

void GrubCustomizer::setAboutDialog(View_About& aboutDialog){
	this->aboutDialog = &aboutDialog;
}

void GrubCustomizer::setThreadController(ThreadController& threadController) {
	this->threadController = &threadController;
}

void GrubCustomizer::setEntryNameMapper(Mapper_EntryName& mapper) {
	this->entryNameMapper = &mapper;
}

ThreadController& GrubCustomizer::getThreadController() {
	if (this->threadController == NULL) {
		throw INCOMPLETE;
	}
	return *this->threadController;
}

void GrubCustomizer::showAboutDialog(){
	this->aboutDialog->show();
}


void GrubCustomizer::showInstallDialog(){
	installDlg->show();
}

void GrubCustomizer::installGrub(std::string device){
	this->env.activeThreadCount++;
	installer->threadable_install(device);
	this->env.activeThreadCount--;
	if (this->env.activeThreadCount == 0 && this->env.quit_requested) {
		this->getAllControllers().mainController->exitAction(true);
	}
}

void GrubCustomizer::showMessageGrubInstallCompleted(std::string const& msg){
	installDlg->showMessageGrubInstallCompleted(msg);
}

