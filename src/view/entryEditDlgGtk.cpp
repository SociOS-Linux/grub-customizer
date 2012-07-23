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

#include "entryEditDlgGtk.h"

EntryEditDlgGtk::EntryEditDlgGtk() {
	this->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	this->add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	this->set_default_size(400, 300);
	Gtk::VBox& vbMain = *this->get_vbox();
	vbMain.add(this->tabbox);
	tabbox.append_page(this->scrSource, gettext("Source"));
	scrSource.add(this->tvSource);

	this->signal_response().connect(sigc::mem_fun(this, &EntryEditDlgGtk::signal_response_action));
}

void EntryEditDlgGtk::setSourcecode(std::string const& source) {
	std::string optimizedSource = str_replace("\n\t", "\n", source);
	if (optimizedSource[0] == '\t') {
		optimizedSource = optimizedSource.substr(1);
	}
	this->tvSource.get_buffer()->set_text(optimizedSource);
}

void EntryEditDlgGtk::show() {
	Gtk::Window::show_all();
}

void EntryEditDlgGtk::hide() {
	Gtk::Window::hide();
}

void EntryEditDlgGtk::signal_response_action(int response_id) {
	if (response_id == Gtk::RESPONSE_OK){
//		eventListener->entryAddDlg_applied();
	}
	this->hide();
}
