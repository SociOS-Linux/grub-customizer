#include "eventListener_view.h"

EventListenerView::EventListenerView(GtkClient& presenter)
	: EventListener_abstract(presenter)
{
}

void EventListenerView::settings_dialog_request(){
	presenter.showSettingsDlg();
}

void EventListenerView::reload_request(){
	Glib::Thread::create(sigc::bind(sigc::mem_fun(&presenter, &GtkClient::load), true), false);
}

void EventListenerView::save_request(){
	presenter.save();
}

void EventListenerView::rootSelectorCompleted(){
	Glib::Thread::create(sigc::bind(sigc::mem_fun(&presenter, &GtkClient::load), false), false);
}

void EventListenerView::entry_rename_request(Rule* rule, std::string const& newName){
	presenter.renameEntry(rule, newName);
}

void EventListenerView::rootSelector_request(){
	presenter.startRootSelector();
}

void EventListenerView::installDialogRequest(){
	presenter.showInstallDialog();
}

void EventListenerView::installGrub_request(std::string const& device){
	presenter.installGrub(device);
}

void EventListenerView::scriptAddDlg_requested(){
	presenter.showScriptAddDlg();
}

void EventListenerView::scriptAddDlg_applied(){
	presenter.addScriptFromScriptAddDlg();
}

void EventListenerView::scriptSelected(){
	presenter.updateScriptAddDlgPreview();
}
