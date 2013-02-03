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

View_Gtk_Trash::View_Gtk_Trash()
	: deleteButton(NULL) {
	this->set_title(gettext("Add entry from trash"));
	this->set_icon_name("grub-customizer");
	this->set_default_size(650, 500);
	Gtk::Box* vbEntryAddDlg = this->get_vbox();
	vbEntryAddDlg->pack_start(frmList);
	frmList.set_label(gettext("Removed items"));
	frmList.set_shadow_type(Gtk::SHADOW_NONE);
	frmList.add(scrEntryBox);
	scrEntryBox.add(list);
	scrEntryBox.set_min_content_width(250);
	
	deleteButton = this->add_button(gettext("Delete custom entries"), Gtk::RESPONSE_REJECT);
	deleteButton->set_no_show_all(true);
	this->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	this->add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	this->signal_response().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_entryAddDlg_response));

	list.set_tooltip_column(0);

	list.ellipsizeMode = Pango::ELLIPSIZE_END;
}

void View_Gtk_Trash::setEventListener(TrashController& eventListener){
	this->eventListener = &eventListener;
}

void View_Gtk_Trash::clear(){
	list.refTreeStore->clear();
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

std::list<Entry*> View_Gtk_Trash::getSelectedEntries(){
	std::list<Entry*> result;
	std::vector<Gtk::TreePath> pathes = list.get_selection()->get_selected_rows();
	for (std::vector<Gtk::TreePath>::iterator pathIter = pathes.begin(); pathIter != pathes.end(); pathIter++) {
		Gtk::TreeModel::iterator elementIter = list.refTreeStore->get_iter(*pathIter);
		result.push_back((*elementIter)[list.treeModel.relatedRule]);
	}
	return result;
}

void View_Gtk_Trash::addItem(View_Model_ListItem<Entry, Script> const& listItem){
	this->list.addListItem(listItem, this->options, *this);
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

Gtk::Widget& View_Gtk_Trash::getList() {
	Gtk::Box* vbEntryAddDlg = this->get_vbox();
	vbEntryAddDlg->remove(this->frmList);
	return this->frmList;
}

void View_Gtk_Trash::setOptions(std::map<ViewOption, bool> const& viewOptions) {
	this->options = viewOptions;
}
