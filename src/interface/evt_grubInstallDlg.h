#ifndef EVENTLISTENER_GRUB_INSTALL_DLG_INCLUDED
#define EVENTLISTENER_GRUB_INSTALL_DLG_INCLUDED
#include <string>

class EventListener_grubInstallDlg {
public:
	virtual void installGrub_request(std::string const& device)=0;
};

#endif
