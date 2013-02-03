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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef ENTRY_ADD_DLG_INCLUDED
#define ENTRY_ADD_DLG_INCLUDED
#include <gtkmm.h>
#include "../../Controller/TrashController.h"
#include "../Trash.h"
#include <libintl.h>
#include "../../lib/CommonClass.h"
#include "../../lib/Type.h"
#include "Element/List.h"

class View_Gtk_Trash : public Gtk::Dialog, public View_Trash, public CommonClass {
	Gtk::Dialog scriptAddDlg;
	Gtk::ScrolledWindow scrEntryBox;
	View_Gtk_Element_List<Entry, Script> list;
	Gtk::Button* deleteButton;
	Gtk::Frame frmList;

	TrashController* eventListener;
	std::map<ViewOption, bool> options;
public:
	View_Gtk_Trash();
	void setEventListener(TrashController& eventListener);
	void signal_entryAddDlg_response(int response_id);
	void clear();
	std::list<Entry*> getSelectedEntries();
	void addItem(View_Model_ListItem<Entry, Script> const& listItem);
	void setDeleteButtonEnabled(bool val);
	void show();
	void hide();
	void askForDeletion(std::list<std::string> const& names);
	Gtk::Widget& getList();
	void setOptions(std::map<ViewOption, bool> const& viewOptions);
};

#endif
