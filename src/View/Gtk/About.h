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

#ifndef GC_ABOUNTDIALOG_GTK_INCLUDED
#define GC_ABOUNTDIALOG_GTK_INCLUDED
#include <gtkmm.h>
#include "../../config.h"
#include <libintl.h>
#include "../../interface/aboutDialog.h"
#include "../../presenter/commonClass.h"

class View_Gtk_About : public Gtk::AboutDialog, public AboutDialog, public CommonClass {
	Glib::ustring appName, appVersion;
	std::vector<Glib::ustring> authors;
	std::vector<Glib::ustring> artists;

	void signal_about_dlg_response(int response_id);
public:
	View_Gtk_About();
	void show();
};

#endif
