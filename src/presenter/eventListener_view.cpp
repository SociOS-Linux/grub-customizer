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

void EventListenerView::removeProxy_requested(Proxy* p){
	presenter.removeProxy(p);
}

bool EventListenerView::exitRequest(){
	return presenter.quit();
}

void EventListenerView::signal_script_state_toggled(void* script){
	presenter.syncProxyState(script);
}

void EventListenerView::signal_entry_state_toggled(Rule* entry){
	presenter.syncRuleState(entry);
}

void EventListenerView::signal_entry_renamed(Rule* entry){
	presenter.syncRuleName(entry);
}

void EventListenerView::ruleSwap_requested(Rule* a, Rule* b){
	presenter.swapRules(a,b);
}
void EventListenerView::proxySwap_requested(Proxy* a, Proxy* b){
	presenter.swapProxies(a,b);
}
