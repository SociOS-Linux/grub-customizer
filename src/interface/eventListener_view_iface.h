#ifndef EVENTLISTENER_VIEW_IFACE
#define EVENTLISTENER_VIEW_IFACE
#include "../model/rule.h"
#include "../model/proxy.h"
#include <string>

class EventListenerView_iface {
	public:
	virtual void settings_dialog_request()=0;
	virtual void reload_request()=0;
	virtual void rootSelectorCompleted()=0;
	virtual void save_request()=0;
	virtual void rootSelector_request()=0;
	virtual void installDialogRequest()=0;
	virtual void installGrub_request(std::string const& device)=0;
	virtual void scriptAddDlg_requested()=0;
	virtual void scriptAddDlg_applied()=0;
	virtual void scriptSelected()=0;
	virtual void removeProxy_requested(Proxy* p)=0;
	virtual bool exitRequest()=0;

	virtual void signal_script_state_toggled(void* script)=0;
	virtual void signal_entry_state_toggled(Rule* entry)=0;
	virtual void signal_entry_renamed(Rule* entry)=0;
	
	virtual void ruleSwap_requested(Rule* a, Rule* b)=0;
	virtual void proxySwap_requested(Proxy* a, Proxy* b)=0;
};

#endif
