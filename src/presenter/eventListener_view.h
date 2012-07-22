#ifndef EVENTLISTENER_VIEW_INCLUDED
#define EVENTLISTENER_VIEW_INCLUDED

#include "gtk-client.h"
#include "../interface/eventListener_view_iface.h"
#include "eventListener_abstract.h"

class EventListenerView : public EventListener_abstract, public EventListenerView_iface {
public:
	EventListenerView(GtkClient& presenter);
	void settings_dialog_request();
	void reload_request();
	void save_request();
	void rootSelectorCompleted();
	void rootSelector_request();
	void installDialogRequest();
	void installGrub_request(std::string const& device);
	void scriptAddDlg_requested();
	void scriptAddDlg_applied();
	void scriptSelected();
	void removeProxy_requested(Proxy* p);
	bool exitRequest();
	
	void signal_script_state_toggled(void* script);
	void signal_entry_state_toggled(Rule* entry);
	void signal_entry_renamed(Rule* entry);
	
	void ruleSwap_requested(Rule* a, Rule* b);
	void proxySwap_requested(Proxy* a, Proxy* b);
};

#endif

