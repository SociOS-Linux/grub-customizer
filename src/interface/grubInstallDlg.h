#ifndef GRUBINSTALLDLG_H_
#define GRUBINSTALLDLG_H_
#include "evt_grubInstallDlg.h"

class GrubInstallDlg {
public:
	virtual void show()=0;
	virtual void showMessageGrubInstallCompleted(std::string const& msg)=0;
	virtual void setEventListener(EventListener_grubInstallDlg& eventListener)=0;
};

#endif
