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

template<typename TItem, typename TWrapper>
class View_Gtk_Element_List : public Gtk::TreeView {
public:
	struct TreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> text;
		Gtk::TreeModelColumn<TItem*> relatedRule;
		Gtk::TreeModelColumn<TWrapper*> relatedScript;
		Gtk::TreeModelColumn<bool> is_other_entries_marker;
		Gtk::TreeModelColumn<bool> is_renamable;
		Gtk::TreeModelColumn<bool> is_renamable_real;
		Gtk::TreeModelColumn<bool> is_editable;
		Gtk::TreeModelColumn<bool> is_sensitive;
		Gtk::TreeModelColumn<bool> is_activated;
		Gtk::TreeModelColumn<bool> is_toplevel;
		Gtk::TreeModelColumn<Pango::EllipsizeMode> ellipsize;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;

		TreeModel(){
			this->add(name);
			this->add(text);
			this->add(relatedRule);
			this->add(relatedScript);
			this->add(is_other_entries_marker);
			this->add(is_renamable);
			this->add(is_renamable_real);
			this->add(is_editable);
			this->add(is_activated);
			this->add(is_sensitive);
			this->add(is_toplevel);
			this->add(icon);
			this->add(ellipsize);
		}
	};
	TreeModel treeModel;
	Glib::RefPtr<Gtk::TreeStore> refTreeStore;
	Gtk::CellRendererPixbuf pixbufRenderer;
	Gtk::CellRendererToggle toggleRenderer;
	Gtk::CellRendererText textRenderer;
	Gtk::TreeViewColumn mainColumn;

	View_Gtk_Element_List(){
		refTreeStore = Gtk::TreeStore::create(treeModel);
		this->set_model(refTreeStore);

		this->append_column(this->mainColumn);
		this->mainColumn.pack_start(pixbufRenderer, false);
		this->mainColumn.add_attribute(pixbufRenderer.property_pixbuf(), treeModel.icon);
		this->mainColumn.pack_start(toggleRenderer, false);
		this->mainColumn.add_attribute(toggleRenderer.property_sensitive(), treeModel.is_sensitive);
		toggleRenderer.set_visible(false);
		this->mainColumn.add_attribute(toggleRenderer.property_active(), treeModel.is_activated);
		this->mainColumn.pack_start(this->textRenderer, true);
		this->mainColumn.add_attribute(this->textRenderer.property_markup(), treeModel.text);
		this->mainColumn.add_attribute(this->textRenderer.property_editable(), treeModel.is_renamable);
		this->mainColumn.set_spacing(10);

		this->mainColumn.add_attribute(this->textRenderer.property_ellipsize(), treeModel.ellipsize);

		this->set_headers_visible(false);
		this->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
		this->set_rubber_banding(true);
	}
};


#endif /* LIST_H_ */
