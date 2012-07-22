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

#ifndef SCRIPT_ADD_DLG_INCLUDED
#define SCRIPT_ADD_DLG_INCLUDED
#include <gtkmm.h>
#include "../interface/evt_scriptAddDlg.h"
#include "../interface/scriptAddDlg.h"
#include <libintl.h>

class ScriptAddDlgGtk : public Gtk::Dialog, public ScriptAddDlg {
	Gtk::Dialog scriptAddDlg;
	Gtk::VBox vbScriptPreview;
	Gtk::HBox hbScriptSelection;
	Gtk::ComboBoxText cbScriptSelection;
	Gtk::ListViewText lvScriptPreview;
	Gtk::ScrolledWindow scrScriptPreview;
	Gtk::Label lblScriptSelection;
	Gtk::Label lblScriptPreview;
	EventListener_scriptAddDlg* eventListener;
public:
	ScriptAddDlgGtk();
	void setEventListener(EventListener_scriptAddDlg& eventListener);
	void signal_scriptAddDlg_response(int response_id);
	void clear();
	void addItem(Glib::ustring const& text);
	int getSelectedEntryIndex();
	void signal_script_selection_changed();
	void clearPreview();
	void addToPreview(Glib::ustring const& name);
	void show();
	void hide();
};

#endif
