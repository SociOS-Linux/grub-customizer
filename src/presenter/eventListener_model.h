#ifndef EVENTLISTENER_MODEL_INCLUDED
#define EVENTLISTENER_MODEL_INCLUDED
#include "gtk-client.h"
#include "../interface/eventListener_model_iface.h"

class EventListenerModel : public EventListenerModel_iface {
	GtkClient& presenter;
public:
	EventListenerModel(GtkClient& presenter);
	void entryListUpdate();
	void saveProgressChanged();
	void threadDied();
};
#endif
