#ifndef EVENTLISTENER_GRUB_INSTALL_DLG_INCLUDED
#define EVENTLISTENER_GRUB_INSTALL_DLG_INCLUDED
#include <string>

/**
 * base class to be implemented by grub-install event listeners
 */
class EventListener_grubInstallDlg {
public:
	//should fire when the user has chosen the device and wants to install it now
	virtual void installGrub_request(std::string const& device)=0;
};

#endif
