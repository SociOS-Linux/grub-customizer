/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SCRIPTADDDLG_H_
#define SCRIPTADDDLG_H_

#include "evt_scriptAddDlg.h"
/**
 * Interface to be implemented by dialogs which lets the user adding scripts
 */
class EntryAddDlg {
public:
	//assigns the event listener
	virtual void setEventListener(EventListener_scriptAddDlg& eventListener)=0;
	//clear the script menu
	virtual void clear()=0;
	//gets the index of the selected script item
	virtual int getSelectedEntryIndex()=0;
	//removes all preview items
	virtual void clearPreview()=0;
	//adds a new item
	virtual void addItem(std::string const& name, bool isPlaceholder, std::string const& scriptName, void* relatedEntry)=0;
	//show this dialog
	virtual void show()=0;
	//thise this dialog
	virtual void hide()=0;
};

#endif
