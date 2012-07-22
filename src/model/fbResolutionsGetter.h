#ifndef FB_RESOLUTIONS_GETTER
#define FB_RESOLUTIONS_GETTER
#include <string>
#include <list>
#include <iostream>
#include <cstdio>
#include "../interface/eventListener_view_iface.h"

class FbResolutionsGetter {
	std::list<std::string> data;
	EventListenerView_iface* eventListener;
public:
	FbResolutionsGetter();
	const std::list<std::string>& getData() const;
	void load();
	void setEventListener(EventListenerView_iface& eventListener);
};

#endif
