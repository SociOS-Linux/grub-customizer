#ifndef GRUBINSTALLDLG_H_
#define GRUBINSTALLDLG_H_
#include "evt_grubInstallDlg.h"

/**
 * Interface for dialogs which helps users to install grub into the MBR
 */
class GrubInstallDlg {
public:
	//show this dialog
	virtual void show()=0;
	//show the information that grub has been installed completely
	virtual void showMessageGrubInstallCompleted(std::string const& msg)=0;
	//assign the event listener
	virtual void setEventListener(EventListener_grubInstallDlg& eventListener)=0;
};

#endif
