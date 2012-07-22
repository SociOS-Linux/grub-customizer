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

EventListener::EventListener(GrubCustomizer& presenter)
	: presenter(presenter)
{
}

void EventListener::settings_dialog_request(){
	presenter.showSettingsDlg();
}

void EventListener::reload_request(){
	presenter.reload();
}

void EventListener::save_request(){
	presenter.save();
}

void EventListener::rootSelectorCompleted(){
	Glib::Thread::create(sigc::bind(sigc::mem_fun(&presenter, &GrubCustomizer::load), false), false);
}


void EventListener::rootSelector_request(){
	presenter.showPartitionChooser();
}

void EventListener::installDialogRequest(){
	presenter.showInstallDialog();
}

void EventListener::installGrub_request(std::string const& device){
	Glib::Thread::create(sigc::bind<std::string>(sigc::mem_fun(&presenter, &GrubCustomizer::installGrub), device), false);
}

void EventListener::scriptAddDlg_applied(){
	presenter.addScriptFromScriptAddDlg();
}

void EventListener::scriptAddDlg_requested(){
	presenter.showScriptAddDlg();
}

void EventListener::scriptSelected(){
	presenter.updateScriptAddDlgPreview();
}

void EventListener::removeProxy_requested(void* p){
	presenter.removeProxy((Proxy*)p);
}

void EventListener::exitRequest(){
	return presenter.quit();
}

void EventListener::signal_script_state_toggled(void* script){
	presenter.syncProxyState((Proxy*)script);
}

void EventListener::signal_entry_state_toggled(void* entry){
	presenter.syncRuleState((Rule*)entry);
}

void EventListener::signal_entry_renamed(void* entry){
	presenter.syncRuleName((Rule*)entry);
}

void EventListener::ruleSwap_requested(void* a, void* b){
	presenter.swapRules((Rule*)a,(Rule*)b);
}
void EventListener::proxySwap_requested(void* a, void* b){
	presenter.swapProxies((Proxy*)a,(Proxy*)b);
}

void EventListener::ruleSelected(void* rule){
	presenter.showRuleInfo((Rule*)rule);
}
void EventListener::proxySelected(void* proxy){
	presenter.showProxyInfo((Proxy*)proxy);
}

void EventListener::burgSwitcher_cancelled(){
	presenter.handleCancelResponse();
}
void EventListener::burgSwitcher_response(bool burgChosen){
	presenter.init(burgChosen ? GrubEnv::BURG_MODE : GrubEnv::GRUB_MODE);
}

void EventListener::partitionChooserQuestion_response(bool is_positive){
	presenter.hidePartitionChooserQuestion();
	if (is_positive)
		presenter.showPartitionChooser();
	else
		presenter.handleCancelResponse();
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
void EventListener::backgroundChange_requested(){
	presenter.updateBackgroundImage();
}
void EventListener::backgroundCopy_requested(){
	presenter.copyBackgroundImageToGrubDirectory();
}
void EventListener::backgroundRemove_requested(){
	presenter.removeBackgroundImage();
}

void EventListener::settings_dialog_hide_request(){
	presenter.hideSettingsDialog();
}

//partition chooser
void EventListener::partitionChooser_applied(){
	presenter.applyPartitionChooser();
}

void EventListener::partitionChooser_cancelled(){
	presenter.cancelPartitionChooser();
}

void EventListener::rootFsMount_request(){
	presenter.mountRootFs();
}

void EventListener::rootFsUmount_request(){
	presenter.umountRootFs();
}

void EventListener::submountpoint_mount_request(std::string const& mountpoint){
	presenter.mountSubmountpoint(mountpoint);
}

void EventListener::submountpoint_umount_request(std::string const& mountpoint){
	presenter.umountSubmountpoint(mountpoint);
}


void EventListener::loadProgressChanged(){
	presenter.syncEntryList();
}

void EventListener::saveProgressChanged(){
	presenter.updateSaveProgress();
}

void EventListener::grubInstallCompleted(std::string const& msg){
	presenter.showMessageGrubInstallCompleted(msg);
}


void EventListener::fb_resolutions_loaded(){
	presenter.updateSettingsDlgResolutionList();
}
