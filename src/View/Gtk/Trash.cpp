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
	: deleteButton(NULL), micRestore(Gtk::Stock::ADD), bttRestore(Gtk::Stock::UNDELETE) {
	this->set_title(gettext("Add entry from trash"));
	this->set_icon_name("grub-customizer");
	this->set_default_size(650, 500);
	Gtk::Box* vbEntryAddDlg = this->get_vbox();
	vbEntryAddDlg->pack_start(frmList);
	frmList.set_label(gettext("Removed items"));
	frmList.set_shadow_type(Gtk::SHADOW_NONE);
	frmList.add(vbList);
	vbList.pack_start(scrEntryBox);
	vbList.pack_start(bttRestore, Gtk::PACK_SHRINK);
	scrEntryBox.add(list);
	scrEntryBox.set_min_content_width(250);
	
	bttRestore.set_label(gettext("_Restore"));
	bttRestore.set_use_underline(true);
	bttRestore.set_tooltip_text(gettext("Restore selected entries"));
	bttRestore.set_border_width(5);
	bttRestore.set_sensitive(false);

	deleteButton = this->add_button(gettext("Delete custom entries"), Gtk::RESPONSE_REJECT);
	deleteButton->set_no_show_all(true);
	this->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	this->add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	this->signal_response().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_entryAddDlg_response));
	this->list.get_selection()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_treeview_selection_changed));
	this->list.signal_row_activated().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_item_dblClick));
	this->list.signal_button_press_event().connect_notify(sigc::mem_fun(this, &View_Gtk_Trash::signal_button_press));
	this->list.signal_popup_menu().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_popup));
	this->micRestore.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Trash::restore_button_click));
	this->bttRestore.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Trash::restore_button_click));

	list.set_tooltip_column(1);

	list.ellipsizeMode = Pango::ELLIPSIZE_END;

	this->miContext.set_submenu(this->contextMenu);
	this->contextMenu.attach(this->micRestore, 0, 1, 0, 1);
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

void View_Gtk_Trash::signal_item_dblClick(Gtk::TreeModel::Path const& path, Gtk::TreeViewColumn* column) {
	this->list.get_selection()->unselect_all();
	this->list.get_selection()->select(path);
	eventListener->applyAction();
	this->hide();
}

void View_Gtk_Trash::restore_button_click() {
	eventListener->applyAction();
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
	this->miContext.show_all();
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

void View_Gtk_Trash::selectEntries(std::list<Entry*> const& entries) {
	this->list.selectRules(entries);
}

void View_Gtk_Trash::setRestoreButtonSensitivity(bool sensitivity) {
	this->bttRestore.set_sensitive(sensitivity);
}

void View_Gtk_Trash::signal_treeview_selection_changed() {
	this->eventListener->updateSelectionAction(this->list.getSelectedRules());
}

void View_Gtk_Trash::signal_button_press(GdkEventButton *event) {
	if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
		contextMenu.show_all();
		contextMenu.popup(event->button, event->time);
	}
}

bool View_Gtk_Trash::signal_popup() {
	contextMenu.show_all();
	contextMenu.popup(0, gdk_event_get_time(NULL));
	return true;
}
