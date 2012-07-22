#ifndef EVENTLISTENER_SCRIPTADDDLG_INCLUDED
#define EVENTLISTENER_SCRIPTADDDLG_INCLUDED

/**
 * base class to be implemented by event listeners for script add dialogs
 */
class EventListener_scriptAddDlg {
public:
	//user wants to add the selected script instance
	virtual void scriptAddDlg_applied()=0;
	//user has chosen a script
	virtual void scriptSelected()=0;
};

#endif
