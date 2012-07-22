#ifndef EVENTLISTENER_MODEL_INCLUDED
#define EVENTLISTENER_MODEL_INCLUDED

#include <string>
class EventListener_model {
public:
	virtual void entryListUpdate()=0;
	virtual void saveProgressChanged()=0;
	virtual void grubInstallCompleted(std::string const& msg)=0;

	virtual void fb_resolutions_loaded()=0;
};

#endif
