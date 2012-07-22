#ifndef SCRIPTADDDLG_H_
#define SCRIPTADDDLG_H_

#include "evt_scriptAddDlg.h"
/**
 * Interface to be implemented by dialogs which lets the user adding scripts
 */
class ScriptAddDlg {
public:
	//assigns the event listener
	virtual void setEventListener(EventListener_scriptAddDlg& eventListener)=0;
	//clear the script menu
	virtual void clear()=0;
	//add item to the end of the script menu
	virtual void addItem(Glib::ustring const& text)=0;
	//gets the index of the selected script item
	virtual int getSelectedEntryIndex()=0;
	//removes all preview items
	virtual void clearPreview()=0;
	//adds a new item to the end of the preview
	virtual void addToPreview(Glib::ustring const& name)=0;
	//show this dialog
	virtual void show()=0;
	//thise this dialog
	virtual void hide()=0;
};

#endif
