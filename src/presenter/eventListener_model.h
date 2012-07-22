#ifndef EVENTLISTENER_MODEL_INCLUDED
#define EVENTLISTENER_MODEL_INCLUDED
#include "gtk-client.h"
#include "../interface/eventListener_model_iface.h"
#include "eventListener_abstract.h"

class EventListenerModel : public EventListener_abstract, public EventListenerModel_iface {
public:
	EventListenerModel(GtkClient& presenter);
	void entryListUpdate();
	void saveProgressChanged();
	void threadDied();
	void grubInstallCompleted(std::string const& msg);

	void fb_resolutions_loaded();
};
#endif
