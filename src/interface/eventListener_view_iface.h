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
	virtual void entry_rename_request(Rule* rule, std::string const& newName)=0;
	virtual void rootSelector_request()=0;
	virtual void installDialogRequest()=0;
	virtual void installGrub_request(std::string const& device)=0;
	virtual void scriptAddDlg_requested()=0;
	virtual void scriptAddDlg_applied()=0;
	virtual void scriptSelected()=0;
	virtual void removeProxy_requested(Proxy* p)=0;
};

#endif
