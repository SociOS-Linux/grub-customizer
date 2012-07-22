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
	void entry_rename_request(Rule* rule, std::string const& newName);
	void rootSelector_request();
	void installDialogRequest();
	void installGrub_request(std::string const& device);
	void scriptAddDlg_requested();
	void scriptAddDlg_applied();
	void scriptSelected();
};

#endif

