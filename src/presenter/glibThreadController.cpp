#include "glibThreadController.h"

GlibThreadController::GlibThreadController(GrubCustomizer& app)
	: app(app)
{
	disp_sync_load.connect(sigc::mem_fun(&this->app, &GrubCustomizer::syncListView_load));
	disp_sync_save.connect(sigc::mem_fun(&this->app, &GrubCustomizer::syncListView_save));
	disp_thread_died.connect(sigc::mem_fun(&this->app, &GrubCustomizer::die));
	disp_settings_loaded.connect(sigc::mem_fun(&this->app, &GrubCustomizer::activateSettingsBtn));
	disp_updateSettingsDlgResolutionList.connect(sigc::mem_fun(&this->app, &GrubCustomizer::updateSettingsDlgResolutionList_dispatched));
}

void GlibThreadController::syncEntryList(){
	this->disp_sync_load();
}

void GlibThreadController::updateSaveProgress(){
	this->disp_sync_save();
}

void GlibThreadController::updateSettingsDlgResolutionList(){
	this->disp_updateSettingsDlgResolutionList();
}

void GlibThreadController::showThreadDiedError() {
	this->disp_thread_died();
}

void GlibThreadController::enableSettings() {
	this->disp_settings_loaded();
}

void GlibThreadController::startLoadThread(bool preserveConfig) {
	Glib::Thread::create(sigc::bind(sigc::mem_fun(&this->app, &GrubCustomizer::load), preserveConfig), false);
}

void GlibThreadController::startSaveThread() {
	Glib::Thread::create(sigc::mem_fun(&this->app, &GrubCustomizer::save_thread), false);
}

void GlibThreadController::startFramebufferResolutionLoader() {
	Glib::Thread::create(sigc::mem_fun(&this->app.getFbResolutionsGetter(), &FbResolutionsGetter::load), false);
}

void GlibThreadController::startGrubInstallThread(std::string const& device) {
	Glib::Thread::create(sigc::bind<std::string>(sigc::mem_fun(&this->app, &GrubCustomizer::installGrub), device), false);
}

void GlibThreadController::stopApplication() {
	Gtk::Main::quit();
}
