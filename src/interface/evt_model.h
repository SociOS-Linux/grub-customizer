#ifndef EVENTLISTENER_MODEL_INCLUDED
#define EVENTLISTENER_MODEL_INCLUDED

#include <string>

/**
 * base class to be implemented by an event listener which is responsible for all model events
 */
class EventListener_model {
public:
	//some new data is loaded
	virtual void loadProgressChanged()=0;
	//save progress continued
	virtual void saveProgressChanged()=0;
	//grub install completely installed
	virtual void grubInstallCompleted(std::string const& msg)=0;
	//framebuffer resolutions has been loaded
	virtual void fb_resolutions_loaded()=0;
};

#endif
