#ifndef EVENTLISTENER_VIEW_IFACE
#define EVENTLISTENER_VIEW_IFACE
#include "../model/rule.h"
#include <string>

class EventListenerView_iface {
	public:
	virtual void settings_dialog_request()=0;
	virtual void reload_request()=0;
	virtual void rootSelectorCompleted()=0;
	virtual void save_request()=0;
	virtual void entry_rename_request(Rule* rule, std::string const& newName)=0;
	virtual void rootSelector_request()=0;
};

#endif
