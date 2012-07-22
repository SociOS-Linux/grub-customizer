#include "eventListener_view.h"

EventListenerView::EventListenerView(GtkClient& presenter)
	: presenter(presenter)
{
}

void EventListenerView::settings_dialog_request(){
	presenter.showSettingsDlg();
}

void EventListenerView::reload_request(){
	Glib::Thread::create(sigc::bind(sigc::mem_fun(&presenter, &GtkClient::load), true), false);
}

void EventListenerView::save_request(){
	Glib::Thread::create(sigc::mem_fun(&presenter, &GtkClient::save), false);
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

