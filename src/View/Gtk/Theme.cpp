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

#include "Theme.h"

View_Gtk_Theme::View_Gtk_Theme()
	: lvFiles(1, true), lblFileSelection(gettext("_Load file: "), true),
	  tbttAdd(Gtk::Stock::ADD), tbttRemove(Gtk::Stock::REMOVE), event_lock(false)
{
	Gtk::Box& main = *this->get_vbox();
	main.pack_start(hpMain);

	toolbar.add(tbttAdd);
	toolbar.add(tbttRemove);

	hpMain.pack1(vbFiles, Gtk::FILL);
	hpMain.pack2(vbEdit, Gtk::FILL, Gtk::EXPAND);

	vbFiles.pack_start(scrFiles);
	vbFiles.pack_start(toolbar, Gtk::PACK_SHRINK);

	vbEdit.pack_start(scrEdit);
	vbEdit.pack_start(hbFileSelection, Gtk::PACK_SHRINK);

	scrFiles.add(lvFiles);
	scrEdit.add(vbEditInner);
	vbEditInner.pack_start(txtEdit);
	vbEditInner.pack_start(imgPreview);

	hbFileSelection.pack_start(lblFileSelection, Gtk::PACK_SHRINK);
	hbFileSelection.pack_start(fcFileSelection);

	sizeGroupFooter = Gtk::SizeGroup::create(Gtk::SIZE_GROUP_VERTICAL);
	sizeGroupFooter->add_widget(toolbar);
	sizeGroupFooter->add_widget(hbFileSelection);

	tbttAdd.set_is_important(true);

	txtEdit.set_no_show_all(true);

	lblFileSelection.set_mnemonic_widget(fcFileSelection);

	lvFiles.set_column_title(0, gettext("File"));

	scrEdit.set_shadow_type(Gtk::SHADOW_IN);
	scrEdit.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	hbFileSelection.set_border_width(5);
	hbFileSelection.set_spacing(5);

	scrFiles.set_min_content_width(200);
	scrFiles.set_shadow_type(Gtk::SHADOW_IN);
	scrFiles.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	this->add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);

	tbttAdd.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileAddClick));
	tbttRemove.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileRemoveClick));
	lvFiles.get_selection()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileSelected));
	lvFiles.get_model()->signal_row_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileRenamed));
	fcFileSelection.signal_file_set().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileChosen));
	txtEdit.get_buffer()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_textChanged));
}


void View_Gtk_Theme::addFile(std::string const& fileName) {
	event_lock = true;
	lvFiles.append(fileName);
	event_lock = false;
}

void View_Gtk_Theme::clear() {
	event_lock = true;
	lvFiles.clear_items();
	event_lock = false;
}

void View_Gtk_Theme::setText(std::string const& text) {
	event_lock = true;
	imgPreview.hide();
	txtEdit.show();
	txtEdit.get_buffer()->set_text(text);
	event_lock = false;
}

void View_Gtk_Theme::setImage(std::string const& path) {
	event_lock = true;
	txtEdit.hide();
	imgPreview.show();
	imgPreview.set(path);
	event_lock = false;
}

void View_Gtk_Theme::selectFile(std::string const& fileName, bool startEdit) {
	int pos = 0;
	for (Gtk::TreeModel::iterator iter = lvFiles.get_model()->get_iter("0"); iter; iter++) {
		if (lvFiles.get_text(pos) == fileName) {
			lvFiles.set_cursor(lvFiles.get_model()->get_path(iter), *lvFiles.get_column(0), startEdit);
			break;
		}
		pos++;
	}
}

void View_Gtk_Theme::show() {
	this->show_all();
}

void View_Gtk_Theme::signal_fileAddClick() {
	if (!event_lock) {
		// TODO: call event Listener
	}
}

void View_Gtk_Theme::signal_fileRemoveClick() {
	if (!event_lock) {
		// TODO: call event Listener
	}
}

void View_Gtk_Theme::signal_fileSelected() {
	if (!event_lock) {
		// TODO: call event Listener
	}
}

void View_Gtk_Theme::signal_fileRenamed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter) {
	if (!event_lock) {
		// TODO: call event Listener
	}
}

void View_Gtk_Theme::signal_fileChosen() {
	if (!event_lock) {
		// TODO: call event Listener
	}
}

void View_Gtk_Theme::signal_textChanged() {
	if (!event_lock) {
		// TODO: call event Listener
	}
}
