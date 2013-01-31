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

#ifndef LIST_H_
#define LIST_H_
#include <gtkmm.h>
#include "../../../lib/Type.h"

class View_Gtk_Element_List : public Gtk::TreeView {
public:
	struct TreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> text;
		Gtk::TreeModelColumn<Rule*> relatedRule;
		Gtk::TreeModelColumn<Proxy*> relatedScript;
		Gtk::TreeModelColumn<bool> is_other_entries_marker;
		Gtk::TreeModelColumn<bool> is_renamable;
		Gtk::TreeModelColumn<bool> is_renamable_real;
		Gtk::TreeModelColumn<bool> is_editable;
		Gtk::TreeModelColumn<bool> is_sensitive;
		Gtk::TreeModelColumn<bool> is_activated;
		Gtk::TreeModelColumn<bool> is_toplevel;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
		TreeModel();
	};
	TreeModel treeModel;
	Glib::RefPtr<Gtk::TreeStore> refTreeStore;
	Gtk::CellRendererPixbuf pixbufRenderer;
	Gtk::CellRendererToggle toggleRenderer;
	Gtk::CellRendererText textRenderer;
	Gtk::TreeViewColumn mainColumn;
	View_Gtk_Element_List();
};

#endif /* LIST_H_ */
