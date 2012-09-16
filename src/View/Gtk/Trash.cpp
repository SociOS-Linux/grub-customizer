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

#include "Trash.h"

View_Gtk_Trash::View_Gtk_Trash() : deleteButton(NULL) {
	this->set_title(gettext("Add entry from trash"));
	this->set_icon_name("grub-customizer");
	this->set_default_size(650, 500);
	Gtk::Box* vbEntryAddDlg = this->get_vbox();
	vbEntryAddDlg->pack_start(scrEntryBox);
	scrEntryBox.add(iconBox);
	
	this->listStore = Gtk::ListStore::create(iconModel);
	iconBox.set_model(this->listStore);
	this->iconBox.set_text_column(this->iconModel.name);
	this->iconBox.set_pixbuf_column(this->iconModel.icon);

	this->iconBox.set_tooltip_column(2);
	this->iconBox.set_item_width(200);

	this->iconBox.set_selection_mode(Gtk::SELECTION_MULTIPLE);

	this->iconBox.signal_item_activated().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_icon_dblClick));

	deleteButton = this->add_button(gettext("Delete custom entries"), Gtk::RESPONSE_REJECT);
	deleteButton->set_no_show_all(true);
	this->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	this->add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	this->signal_response().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_entryAddDlg_response));
}

void View_Gtk_Trash::setEventListener(TrashController& eventListener){
	this->eventListener = &eventListener;
}

void View_Gtk_Trash::clear(){
	listStore->clear();
}

void View_Gtk_Trash::signal_entryAddDlg_response(int response_id){
	switch (response_id) {
	case Gtk::RESPONSE_OK:
		this->eventListener->applyAction();
		this->hide();
		break;
	case Gtk::RESPONSE_CANCEL:
		this->hide();
		break;
	case Gtk::RESPONSE_REJECT:
		this->eventListener->askForDeletionAction();
		break;
	}
}

void View_Gtk_Trash::signal_icon_dblClick(Gtk::TreeModel::Path path) {
	this->iconBox.select_path(path);
	eventListener->applyAction();
	this->hide();
}

std::list<void*> View_Gtk_Trash::getSelectedEntries(){
	std::list<void*> result;
	std::vector<Gtk::TreePath> pathes = iconBox.get_selected_items();
	for (std::vector<Gtk::TreePath>::iterator pathIter = pathes.begin(); pathIter != pathes.end(); pathIter++) {
		Gtk::TreeModel::iterator elementIter = listStore->get_iter(*pathIter);
		result.push_back((*elementIter)[iconModel.relatedRule]);
	}
	return result;
}

void View_Gtk_Trash::addItem(std::string const& name, bool isPlaceholder, std::string const& scriptName, void* relatedEntry){
	Gtk::TreeModel::iterator iter = this->listStore->append();
	(*iter)[iconModel.name] = name;
	(*iter)[iconModel.icon] = this->iconBox.render_icon(isPlaceholder ? Gtk::Stock::FIND : Gtk::Stock::EXECUTE, Gtk::ICON_SIZE_DND);
	(*iter)[iconModel.description] = name + "\n" + gettext("type: ") + (isPlaceholder ? gettext("placeholder") : gettext("menuentry")) + "\n" + gettext("script: ") + scriptName;
	(*iter)[iconModel.relatedRule] = relatedEntry;
}

void View_Gtk_Trash::setDeleteButtonEnabled(bool val) {
	this->deleteButton->set_visible(val);
}

void View_Gtk_Trash::show(){
	this->show_all();
}

void View_Gtk_Trash::hide(){
	this->Gtk::Dialog::hide();
}

void View_Gtk_Trash::askForDeletion(std::list<std::string> const& names) {
	Glib::ustring question = gettext("This deletes the following entries:");
	question += "\n";
	for (std::list<std::string>::const_iterator iter = names.begin(); iter != names.end(); iter++) {
		question += *iter + "\n";
	}

	int response = Gtk::MessageDialog(question, false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK_CANCEL).run();
	if (response == Gtk::RESPONSE_OK) {
		this->eventListener->deleteCustomEntriesAction();
	}
}

View_Gtk_Trash::IconModel::IconModel() {
	this->add(this->name);
	this->add(this->icon);
	this->add(this->description);
	this->add(this->relatedRule);
}
