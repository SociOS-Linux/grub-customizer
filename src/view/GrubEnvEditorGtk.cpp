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

#include "GrubEnvEditorGtk.h"

GrubEnvEditorGtk::GrubEnvEditorGtk()
	: eventListener(NULL), pChooser(NULL), lblPartition(gettext("_Partition:"), true), deviceDataList(NULL),
	  lblType(gettext("_Type:"), true)
{
	this->set_title("Grub Customizer environment setup");
	this->set_icon_name("grub-customizer");

	Gtk::VBox& box = *this->get_vbox();
	box.add(this->tblLayout);

	this->tblLayout.attach(this->lblPartition, 0, 1, 0, 1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);

	this->tblLayout.attach(this->lblType, 0, 1, 1, 2, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
	this->tblLayout.attach(this->cbType, 1, 2, 1, 2, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);

	this->tblLayout.attach(this->separator, 0, 2, 2, 3, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);

	this->cbType.append(gettext("Grub 2"));
	this->cbType.append(gettext("BURG"));
	this->cbType.set_active(0);
	this->cbType.signal_changed().connect(sigc::mem_fun(this, &GrubEnvEditorGtk::signal_bootloaderType_changed));

	lblPartition.set_alignment(Gtk::ALIGN_RIGHT);
	lblType.set_alignment(Gtk::ALIGN_RIGHT);

	this->tblLayout.set_spacings(10);
	this->tblLayout.set_border_width(10);

	this->add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_CLOSE);
	this->add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
}

void GrubEnvEditorGtk::setEventListener(EventListener_grubEnvEditor& eventListener) {
	this->eventListener = &eventListener;
}

void GrubEnvEditorGtk::setDeviceDataList(DeviceDataList_Iface& deviceDataList) {
	this->deviceDataList = &deviceDataList;
}

void GrubEnvEditorGtk::clear() {
	for (std::map<std::string, Gtk::Label*>::iterator iter = this->labelMap.begin(); iter != this->labelMap.end(); iter++) {
		this->tblLayout.remove(*iter->second);
		this->tblLayout.remove(*this->optionMap[iter->first]);
		this->tblLayout.remove(*this->imageMap[iter->first]);
	}
}

void GrubEnvEditorGtk::setEnvSettings(std::map<std::string, std::string> const& props, std::list<std::string> const& requiredProps, std::list<std::string> const& validProps) {
	this->clear();
	int pos = 3;
	for (std::map<std::string, std::string>::const_iterator iter = props.begin(); iter != props.end(); iter++) {
		Gtk::Label* label = Gtk::manage(new Gtk::Label(iter->first + ":"));
		label->set_alignment(Gtk::ALIGN_RIGHT);
		this->tblLayout.attach(*label, 0, 1, pos, pos+1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
		this->labelMap[iter->first] = label;

		Gtk::Entry& entry = *Gtk::manage(new Gtk::Entry());
		entry.set_text(iter->second);
//		entry.signal_changed().connect(sigc::mem_fun(this, &EntryEditDlgGtk::signal_optionsModified));

		this->tblLayout.attach(entry, 1, 2, pos, pos+1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
		label->set_mnemonic_widget(entry);

		this->optionMap[iter->first] = &entry;

		Glib::RefPtr<Gdk::Pixbuf> icon;
		if (std::find(validProps.begin(), validProps.end(), iter->first) != validProps.end()) {
			icon = this->render_icon(Gtk::Stock::OK, Gtk::ICON_SIZE_BUTTON);
		} else if (std::find(requiredProps.begin(), requiredProps.end(), iter->first) != requiredProps.end()) {
			icon = this->render_icon(Gtk::Stock::DIALOG_ERROR, Gtk::ICON_SIZE_BUTTON);
		} else {
			icon = this->render_icon(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_BUTTON);
		}
		Gtk::Image* img = Gtk::manage(new Gtk::Image(icon));
		this->imageMap[iter->first] = img;
		this->tblLayout.attach(*img, 2, 3, pos, pos+1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);

		if (this->is_visible()) {
			this->tblLayout.show_all();
		}
		pos++;
	}
}

void GrubEnvEditorGtk::show() {
	if (this->pChooser != NULL) {
		this->tblLayout.remove(*pChooser);
		delete this->pChooser;
		this->pChooser = NULL;
	}

	this->pChooser = new PartitionChooser_DropDown("", *this->deviceDataList);
	this->tblLayout.attach(*pChooser, 1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
	this->lblPartition.set_mnemonic_widget(*pChooser);

	this->show_all();
}
void GrubEnvEditorGtk::hide() {
	this->hide();
}

void GrubEnvEditorGtk::signal_bootloaderType_changed() {
	this->eventListener->grubEnvEditor_typeChanged(this->cbType.get_active_row_number());
}

