#ifndef EVENTLISTENER_VIEW_IFACE
#define EVENTLISTENER_VIEW_IFACE
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
	virtual void removeProxy_requested(void* p)=0;
	virtual bool exitRequest()=0;

	virtual void signal_script_state_toggled(void* script)=0;
	virtual void signal_entry_state_toggled(void* entry)=0;
	virtual void signal_entry_renamed(void* entry)=0;
	
	virtual void ruleSwap_requested(void* a, void* b)=0;
	virtual void proxySwap_requested(void* a, void* b)=0;
	
	virtual void rootSelector_requested()=0;
	
	virtual void ruleSelected(void* rule)=0;
	virtual void proxySelected(void* proxy)=0;
};

#endif
