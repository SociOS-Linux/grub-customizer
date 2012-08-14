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

#ifndef ENTRY_ADD_DLG_INCLUDED
#define ENTRY_ADD_DLG_INCLUDED
#include <gtkmm.h>
#include "../../interface/evt_entryAddDlg.h"
#include "../Trash.h"
#include <libintl.h>
#include "../../presenter/commonClass.h"

class View_Gtk_Trash : public Gtk::Dialog, public View_Trash, public CommonClass {
	Gtk::Dialog scriptAddDlg;
	Gtk::ScrolledWindow scrEntryBox;
	Gtk::IconView iconBox;

	EventListener_entryAddDlg* eventListener;
public:
	struct IconModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
		Gtk::TreeModelColumn<Glib::ustring> description;
		Gtk::TreeModelColumn<void*> relatedRule;
		IconModel();
	} iconModel;
	Glib::RefPtr<Gtk::ListStore> listStore;
	View_Gtk_Trash();
	void setEventListener(EventListener_entryAddDlg& eventListener);
	void signal_entryAddDlg_response(int response_id);
	void signal_icon_dblClick(Gtk::TreeModel::Path path);
	void clear();
	std::list<void*> getSelectedEntries();
	void addItem(std::string const& name, bool isPlaceholder, std::string const& scriptName,void* relatedRule);
	void show();
	void hide();
};

#endif
