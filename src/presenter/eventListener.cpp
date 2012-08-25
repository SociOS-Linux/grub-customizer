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

#include "eventListener.h"

EventListener::EventListener(GrubCustomizer& presenter, ControllerCollection& controllers)
	: presenter(presenter), _controllers(controllers)
{
}

void EventListener::settings_dialog_request(){
	presenter.showSettingsDlg();
}

void EventListener::rootSelectorCompleted(){
	this->presenter.getThreadController().startLoadThread(false);
}


void EventListener::installGrub_request(std::string const& device){
	this->presenter.getThreadController().startGrubInstallThread(device);
}

void EventListener::entryAddDlg_applied(){
	presenter.addEntryFromEntryAddDlg();
}

void EventListener::entryAddDlg_requested(){
	presenter.showEntryAddDlg();
}

void EventListener::entryEditDlg_requested(void* rule) {
	presenter.showEntryEditDlg(rule);
}

void EventListener::entryCreateDlgRequested() {
	presenter.showEntryCreateDlg();
}

void EventListener::aboutDialog_requested(){
	presenter.showAboutDialog();
}

//settings dialog
void EventListener::setting_row_changed(std::string const& name){
	presenter.updateCustomSetting(name);
}

void EventListener::customRow_insert_requested(){
	presenter.addNewCustomSettingRow();
}
void EventListener::customRow_remove_requested(std::string const& name){
	presenter.removeCustomSettingRow(name);

}

void EventListener::default_entry_predefined_toggeled(){
	presenter.updateDefaultSetting();
}
void EventListener::default_entry_saved_toggeled(){
	presenter.updateDefaultSetting();
}
void EventListener::default_entry_changed(){
	presenter.updateDefaultSetting();
}
void EventListener::showMenu_toggled(){
	presenter.updateShowMenuSetting();
}
void EventListener::osProber_toggled(){
	presenter.updateOsProberSetting();
}
void EventListener::timeout_changed(){
	presenter.updateTimeoutSetting();
}
void EventListener::kernelparams_changed(){
	presenter.updateKernalParams();
}
void EventListener::generateRecovery_toggled(){
	presenter.updateGenerateRecoverySetting();
}
void EventListener::useCustomResolution_toggled(){
	presenter.updateUseCustomResolution();
}
void EventListener::resolution_changed(){
	presenter.updateCustomResolution();
}
void EventListener::colorChange_requested(){
	presenter.updateColorSettings();
}
void EventListener::fontChange_requested() {
	presenter.updateFontSettings(false);
}
void EventListener::fontRemove_requested() {
	presenter.updateFontSettings(true);
}
void EventListener::backgroundChange_requested(){
	presenter.updateBackgroundImage();
}
void EventListener::backgroundRemove_requested(){
	presenter.removeBackgroundImage();
}

void EventListener::settings_dialog_hide_request(){
	presenter.hideSettingsDialog();
}

//partition chooser
void EventListener::submountpoint_mount_request(std::string const& mountpoint){
	presenter.mountSubmountpoint(mountpoint);
}

void EventListener::submountpoint_umount_request(std::string const& mountpoint){
	presenter.umountSubmountpoint(mountpoint);
}


void EventListener::loadProgressChanged(){
	this->_controllers.mainController->syncLoadStateThreadedAction();
}

void EventListener::saveProgressChanged(){
	this->_controllers.mainController->syncSaveStateThreadedAction();
}

void EventListener::grubInstallCompleted(std::string const& msg){
	presenter.showMessageGrubInstallCompleted(msg);
}


void EventListener::fb_resolutions_loaded(){
	presenter.getThreadController().updateSettingsDlgResolutionList();
}

void EventListener::grubEnvEditor_partitionChanged(std::string const& newPartition) {
	presenter.switchPartition(newPartition);
}

void EventListener::grubEnvEditor_typeChanged(int newTypeIndex) {
	presenter.switchBootloaderType(newTypeIndex);
}

void EventListener::grubEnvEditor_optionModified() {
	presenter.updateGrubEnvOptions();
}

void EventListener::grubEnvEditor_cancellationRequested() {
	this->_controllers.mainController->exitAction(true);
}

void EventListener::grubEnvEditor_applyRequested(bool saveConfig) {
	presenter.applyEnvEditor(saveConfig);
}
