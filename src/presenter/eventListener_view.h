#ifndef EVENTLISTENER_VIEW_INCLUDED
#define EVENTLISTENER_VIEW_INCLUDED

#include "gtk-client.h"
#include "../interface/eventListener_view_iface.h"

class EventListenerView : public EventListenerView_iface {
public:
	GtkClient& presenter;
	EventListenerView(GtkClient& presenter);
	void settings_dialog_request();
	void reload_request();
	void save_request();
	void rootSelectorCompleted();
	void entry_rename_request(Rule* rule, std::string const& newName);
	void rootSelector_request();
};

#endif

