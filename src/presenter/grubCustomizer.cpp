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
	  installer(NULL), installDlg(NULL), entryAddDlg(NULL),
	  mountTable(NULL), aboutDialog(NULL),
	 env(env),
	 grubEnvEditor(NULL), threadController(NULL),
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
void GrubCustomizer::setScriptAddDlg(View_Trash& scriptAddDlg){
	this->entryAddDlg = &scriptAddDlg;
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

void GrubCustomizer::setGrubEnvEditor(View_EnvEditor& envEditor) {
	this->grubEnvEditor = &envEditor;
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

void GrubCustomizer::showEnvEditor(bool resetPartitionChooser) {
	this->grubEnvEditor->setEnvSettings(this->env.getProperties(), this->env.getRequiredProperties(), this->env.getValidProperties());
	this->grubEnvEditor->show(resetPartitionChooser);
}


void GrubCustomizer::showAboutDialog(){
	this->aboutDialog->show();
}

void GrubCustomizer::generateSubmountpointSelection(std::string const& prefix){
	this->grubEnvEditor->removeAllSubmountpoints();

	//create new submountpoint checkbuttons
	for (Model_MountTable::const_iterator iter = mountTable->begin(); iter != mountTable->end(); iter++){
		if (iter->mountpoint.length() > prefix.length() && iter->mountpoint.substr(0, prefix.length()) == prefix
		 && iter->mountpoint != prefix + "/dev"
		 && iter->mountpoint != prefix + "/proc"
		 && iter->mountpoint != prefix + "/sys"
		) {
			this->grubEnvEditor->addSubmountpoint(iter->mountpoint.substr(prefix.length()), iter->isMounted);
		}
	}
}

void GrubCustomizer::switchPartition(std::string const& newPartition) {
	if (this->mountTable->getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT).isMounted) {
		this->mountTable->umountAll(PARTCHOOSER_MOUNTPOINT);
		this->mountTable->clear(PARTCHOOSER_MOUNTPOINT);
	}
	this->grubEnvEditor->removeAllSubmountpoints();
	std::string selectedDevice = newPartition;
	if (newPartition != "") {
		mkdir(PARTCHOOSER_MOUNTPOINT, 0755);
		try {
			mountTable->clear(PARTCHOOSER_MOUNTPOINT);
			mountTable->mountRootFs(selectedDevice, PARTCHOOSER_MOUNTPOINT);
			this->env.init(env.burgMode ? Model_Env::BURG_MODE : Model_Env::GRUB_MODE, PARTCHOOSER_MOUNTPOINT);
			this->generateSubmountpointSelection(PARTCHOOSER_MOUNTPOINT);
			this->showEnvEditor();
		}
		catch (Model_MountTable_Mountpoint::Exception const& e) {
			if (e == Model_MountTable_Mountpoint::MOUNT_FAILED){
				this->grubEnvEditor->showErrorMessage(View_EnvEditor::MOUNT_FAILED);
				this->switchPartition("");
			}
		}
		catch (Model_MountTable::Exception const& e) {
			if (e == Model_MountTable::MOUNT_ERR_NO_FSTAB){
				this->grubEnvEditor->showErrorMessage(View_EnvEditor::MOUNT_ERR_NO_FSTAB);
				mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT).umount();
				this->switchPartition("");
			}
		}
	} else {
		this->env.init(env.burgMode ? Model_Env::BURG_MODE : Model_Env::GRUB_MODE, selectedDevice);
		this->showEnvEditor(true);
	}
}

void GrubCustomizer::switchBootloaderType(int newTypeIndex) {
	this->env.init(newTypeIndex == 0 ? Model_Env::GRUB_MODE : Model_Env::BURG_MODE, this->env.cfg_dir_prefix);
	this->showEnvEditor();
}

void GrubCustomizer::updateGrubEnvOptions() {
	this->env.setProperties(this->grubEnvEditor->getEnvSettings());
	this->showEnvEditor();
}

void GrubCustomizer::applyEnvEditor(bool saveConfig){
//	listCfgDlg->setLockState(1|2|8);
//	this->syncSettings();
	this->getAllControllers().settingsController->hideAction();
	entryAddDlg->hide();
	bool isBurgMode = this->grubEnvEditor->getBootloaderType() == 1;
	grubEnvEditor->hide();

	if (saveConfig) {
		this->env.save();
	}
	this->getAllControllers().mainController->reInitAction(isBurgMode);
}


void GrubCustomizer::mountSubmountpoint(std::string const& submountpoint){
	try {
		this->mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT + submountpoint).mount();
	}
	catch (Model_MountTable_Mountpoint::Exception const& e){
		if (e == Model_MountTable_Mountpoint::MOUNT_FAILED){
			this->grubEnvEditor->showErrorMessage(View_EnvEditor::SUB_MOUNT_FAILED);
		}
		this->grubEnvEditor->setSubmountpointSelectionState(submountpoint, false);
		this->grubEnvEditor->show();
	}
}

void GrubCustomizer::umountSubmountpoint(std::string const& submountpoint){
	try {
		this->mountTable->getEntryRefByMountpoint(PARTCHOOSER_MOUNTPOINT + submountpoint).umount();
	}
	catch (Model_MountTable_Mountpoint::Exception const& e){
		if (e == Model_MountTable_Mountpoint::UMOUNT_FAILED){
			this->grubEnvEditor->showErrorMessage(View_EnvEditor::SUB_UMOUNT_FAILED);
		}
		this->grubEnvEditor->setSubmountpointSelectionState(submountpoint, true);
		this->grubEnvEditor->show();
	}
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

void GrubCustomizer::showEntryAddDlg(){
	entryAddDlg->clear();

	std::list<Model_Entry*> removedEntries = this->grublistCfg->getRemovedEntries();
	for (std::list<Model_Entry*>::iterator iter = removedEntries.begin(); iter != removedEntries.end(); iter++) {
		Model_Script* script = this->grublistCfg->repository.getScriptByEntry(**iter);

		std::string name = (*iter)->name;
		name = this->entryNameMapper->map(&**iter, name, script->name);

		entryAddDlg->addItem(name, (*iter)->type != Model_Entry::MENUENTRY, script->name, *iter);
	}

	entryAddDlg->show();
}

void GrubCustomizer::addEntryFromEntryAddDlg(){
	std::list<void*> entries = entryAddDlg->getSelectedEntries();
	this->getAllControllers().mainController->addEntriesAction(entries);
}


void GrubCustomizer::grubEnvSetRootDeviceName(std::string const& rootDevice) {
	this->grubEnvEditor->setRootDeviceName(rootDevice);
}

void GrubCustomizer::grubEnvsetEnvSettings(std::map<std::string, std::string> const& props, std::list<std::string> const& requiredProps, std::list<std::string> const& validProps) {
	this->grubEnvEditor->setEnvSettings(props, requiredProps, validProps);
}

void GrubCustomizer::grubEnvShow(bool resetPartitionChooser = false) {
	this->grubEnvEditor->show();
}
