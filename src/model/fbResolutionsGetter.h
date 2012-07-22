#ifndef FB_RESOLUTIONS_GETTER
#define FB_RESOLUTIONS_GETTER
#include <string>
#include <list>
#include <iostream>
#include <cstdio>
#include "../interface/eventListener_model_iface.h"

class FbResolutionsGetter {
	std::list<std::string> data;
	EventListenerModel_iface* eventListener;
public:
	FbResolutionsGetter();
	const std::list<std::string>& getData() const;
	void load();
	void setEventListener(EventListenerModel_iface& eventListener);
};

#endif
