#ifndef SCRIPTADDDLG_H_
#define SCRIPTADDDLG_H_

#include "evt_scriptAddDlg.h"
class ScriptAddDlg {
public:
	virtual void setEventListener(EventListener_scriptAddDlg& eventListener)=0;
	virtual void signal_scriptAddDlg_response(int response_id)=0;
	virtual void clear()=0;
	virtual void addItem(Glib::ustring const& text)=0;
	virtual int getSelectedEntryIndex()=0;
	virtual void signal_script_selection_changed()=0;
	virtual void clearPreview()=0;
	virtual void addToPreview(Glib::ustring const& name)=0;
	virtual void show()=0;
};

#endif
