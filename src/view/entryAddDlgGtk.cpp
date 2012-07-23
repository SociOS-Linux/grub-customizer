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

#include "entryAddDlgGtk.h"

EntryAddDlgGtk::EntryAddDlgGtk() {
	this->set_title(gettext("Add entry from trash"));
	this->set_icon_name("grub-customizer");
	this->set_default_size(650, 500);
	Gtk::VBox* vbEntryAddDlg = this->get_vbox();
	vbEntryAddDlg->pack_start(scrEntryBox);
	scrEntryBox.add(iconBox);
	
	this->listStore = Gtk::ListStore::create(iconModel);
	iconBox.set_model(this->listStore);
	this->iconBox.set_text_column(this->iconModel.name);
	this->iconBox.set_pixbuf_column(this->iconModel.icon);

	this->iconBox.set_tooltip_column(2);
	this->iconBox.set_item_width(200);

	this->iconBox.set_selection_mode(Gtk::SELECTION_MULTIPLE);

	this->iconBox.signal_item_activated().connect(sigc::mem_fun(this, &EntryAddDlgGtk::signal_icon_dblClick));

	this->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	this->add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	this->signal_response().connect(sigc::mem_fun(this, &EntryAddDlgGtk::signal_entryAddDlg_response));
}

void EntryAddDlgGtk::setEventListener(EventListener_entryAddDlg& eventListener){
	this->eventListener = &eventListener;
}

void EntryAddDlgGtk::clear(){
	listStore->clear();
}

void EntryAddDlgGtk::signal_entryAddDlg_response(int response_id){
	if (response_id == Gtk::RESPONSE_OK){
		eventListener->entryAddDlg_applied();
	}
	this->hide();
}

void EntryAddDlgGtk::signal_icon_dblClick(Gtk::TreeModel::Path path) {
	this->iconBox.select_path(path);
	eventListener->entryAddDlg_applied();
	this->hide();
}

std::list<void*> EntryAddDlgGtk::getSelectedEntries(){
	std::list<void*> result;
	std::vector<Gtk::TreePath> pathes = iconBox.get_selected_items();
	for (std::vector<Gtk::TreePath>::iterator pathIter = pathes.begin(); pathIter != pathes.end(); pathIter++) {
		Gtk::TreeModel::iterator elementIter = listStore->get_iter(*pathIter);
		result.push_back((*elementIter)[iconModel.relatedRule]);
	}
	return result;
}

void EntryAddDlgGtk::addItem(std::string const& name, bool isPlaceholder, std::string const& scriptName, void* relatedEntry){
	Gtk::TreeModel::iterator iter = this->listStore->append();
	(*iter)[iconModel.name] = name;
	(*iter)[iconModel.icon] = this->iconBox.render_icon(isPlaceholder ? Gtk::Stock::FIND : Gtk::Stock::EXECUTE, Gtk::ICON_SIZE_DND);
	(*iter)[iconModel.description] = name + "\n" + gettext("type: ") + (isPlaceholder ? gettext("placeholder") : gettext("menuentry")) + "\n" + gettext("script: ") + scriptName;
	(*iter)[iconModel.relatedRule] = relatedEntry;
}

void EntryAddDlgGtk::show(){
	this->show_all();
}

void EntryAddDlgGtk::hide(){
	this->Gtk::Dialog::hide();
}

EntryAddDlgGtk::IconModel::IconModel() {
	this->add(this->name);
	this->add(this->icon);
	this->add(this->description);
	this->add(this->relatedRule);
}
