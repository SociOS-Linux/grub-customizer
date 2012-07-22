#ifndef EVENTLISTENER_ABSTRACT_INCLUDED
#define EVENTLISTENER_ABSTRACT_INCLUDED
#include "gtk-client.h"
class EventListener_abstract {
protected:
	GtkClient& presenter;
public:
	EventListener_abstract(GtkClient& presenter);
};
#endif
