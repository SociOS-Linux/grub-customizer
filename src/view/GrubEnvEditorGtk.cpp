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
	  lblType(gettext("_Type:"), true), eventLock(true), lblSubmountpoints(gettext("Submountpoints:"))
{
	this->set_title("Grub Customizer environment setup");
	this->set_icon_name("grub-customizer");

	Gtk::VBox& box = *this->get_vbox();
	box.add(this->tblLayout);

	this->tblLayout.attach(this->lblPartition, 0, 1, 0, 1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);

	this->tblLayout.attach(this->lblSubmountpoints, 0, 1, 1, 2);
	this->tblLayout.attach(this->scrSubmountpoints, 1, 2, 1, 2);

	this->tblLayout.attach(this->lblType, 0, 1, 2, 3, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
	this->tblLayout.attach(this->cbType, 1, 2, 2, 3, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);

	this->tblLayout.attach(this->separator, 0, 2, 3, 4, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);

	this->scrSubmountpoints.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	this->scrSubmountpoints.add(this->vbSubmountpoints);
	this->scrSubmountpoints.set_size_request(-1, 50);

	this->lblSubmountpoints.set_no_show_all(true);
	this->scrSubmountpoints.set_no_show_all(true);

	this->cbType.append(gettext("Grub 2"));
	this->cbType.append(gettext("BURG"));
	this->cbType.set_active(0);
	this->cbType.signal_changed().connect(sigc::mem_fun(this, &GrubEnvEditorGtk::signal_bootloaderType_changed));

	lblPartition.set_alignment(Gtk::ALIGN_RIGHT);
	lblType.set_alignment(Gtk::ALIGN_RIGHT);
	lblSubmountpoints.set_alignment(Gtk::ALIGN_RIGHT);

	this->tblLayout.set_spacings(10);
	this->tblLayout.set_border_width(10);

	this->add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_CLOSE);
	this->add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);

	this->signal_response().connect(sigc::mem_fun(this, &GrubEnvEditorGtk::signal_response_action));

	this->eventLock = false;
}

GrubEnvEditorGtk::~GrubEnvEditorGtk() {
	this->tblLayout.remove(*this->pChooser);
	delete this->pChooser;
	this->pChooser = NULL;
}

void GrubEnvEditorGtk::setEventListener(EventListener_grubEnvEditor& eventListener) {
	this->eventListener = &eventListener;
}

void GrubEnvEditorGtk::setDeviceDataList(DeviceDataList_Iface& deviceDataList) {
	this->deviceDataList = &deviceDataList;
}

void GrubEnvEditorGtk::setRootDeviceName(std::string const& rootDeviceName) {
	this->rootDeviceName = rootDeviceName;
}

void GrubEnvEditorGtk::setEnvSettings(std::map<std::string, std::string> const& props, std::list<std::string> const& requiredProps, std::list<std::string> const& validProps) {
	this->eventLock = true;
	int pos = 4;
	for (std::map<std::string, std::string>::const_iterator iter = props.begin(); iter != props.end(); iter++) {
		Gtk::Label* label = NULL;
		if (this->labelMap.find(iter->first) == this->labelMap.end()) {
			label = Gtk::manage(new Gtk::Label(iter->first + ":"));
			label->set_alignment(Gtk::ALIGN_RIGHT);
			this->tblLayout.attach(*label, 0, 1, pos, pos+1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
			this->labelMap[iter->first] = label;
		} else {
			label = this->labelMap[iter->first];
		}

		Gtk::Entry* entry = NULL;
		bool entryCreated = false;
		if (this->optionMap.find(iter->first) == this->optionMap.end()) {
			entry = Gtk::manage(new Gtk::Entry());
			entryCreated = true;
			this->tblLayout.attach(*entry, 1, 2, pos, pos+1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
			label->set_mnemonic_widget(*entry);
			entry->signal_changed().connect(sigc::mem_fun(this, &GrubEnvEditorGtk::signal_optionModified));
			this->optionMap[iter->first] = entry;
		} else {
			entry = this->optionMap[iter->first];
		}

		if (entry->get_text() != iter->second) {
			entry->set_text(iter->second);
		}

		Gtk::Image* img = NULL;
		if (this->imageMap.find(iter->first) == this->imageMap.end()) {
			img = Gtk::manage(new Gtk::Image());
			this->tblLayout.attach(*img, 2, 3, pos, pos+1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
			this->imageMap[iter->first] = img;
		} else {
			img = this->imageMap[iter->first];
		}

		Glib::RefPtr<Gdk::Pixbuf> icon;
		if (std::find(validProps.begin(), validProps.end(), iter->first) != validProps.end()) {
			icon = this->render_icon(Gtk::Stock::OK, Gtk::ICON_SIZE_BUTTON);
		} else if (std::find(requiredProps.begin(), requiredProps.end(), iter->first) != requiredProps.end()) {
			icon = this->render_icon(Gtk::Stock::DIALOG_ERROR, Gtk::ICON_SIZE_BUTTON);
		} else {
			icon = this->render_icon(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_BUTTON);
		}

		img->set(icon);

		if (this->is_visible()) {
			this->tblLayout.show_all();
		}
		pos++;
	}
	this->eventLock = false;
}

std::map<std::string, std::string> GrubEnvEditorGtk::getEnvSettings() {
	std::map<std::string, std::string> result;
	for (std::map<std::string, Gtk::Entry*>::iterator iter = this->optionMap.begin(); iter != this->optionMap.end(); iter++) {
		result[iter->first] = iter->second->get_text();
	}
	return result;
}

int GrubEnvEditorGtk::getBootloaderType() const {
	return this->cbType.get_active_row_number();
}

void GrubEnvEditorGtk::show(bool resetPartitionChooser) {
	this->eventLock = true;
	if (this->pChooser != NULL) {
		this->tblLayout.remove(*pChooser);
	}

	if (!this->pChooser) {
		this->pChooser = new PartitionChooser_DropDown("", *this->deviceDataList, true, this->rootDeviceName);
		this->pChooser->signal_changed().connect(sigc::mem_fun(this, &GrubEnvEditorGtk::signal_partitionChanged));
	}
	if (resetPartitionChooser) {
		this->pChooser->set_active(0);
	}

	this->tblLayout.attach(*pChooser, 1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
	this->lblPartition.set_mnemonic_widget(*pChooser);

	this->show_all();

	this->eventLock = false;
}
void GrubEnvEditorGtk::hide() {
	Gtk::Dialog::hide();
}

void GrubEnvEditorGtk::removeAllSubmountpoints() {
	for (std::map<std::string, Gtk::CheckButton*>::iterator iter = this->subMountpoints.begin(); iter != this->subMountpoints.end(); iter++) {
		this->vbSubmountpoints.remove(*iter->second);
		delete iter->second;
	}
	this->subMountpoints.clear();

	this->scrSubmountpoints.hide();
	this->lblSubmountpoints.hide();
}

void GrubEnvEditorGtk::addSubmountpoint(std::string const& name, bool isActive) {
	Gtk::CheckButton* cb = new Gtk::CheckButton(name);
	cb->set_active(isActive);
	cb->signal_toggled().connect(sigc::bind<Gtk::CheckButton&>(sigc::mem_fun(this, &GrubEnvEditorGtk::signal_submountpointToggled), *cb));
	this->vbSubmountpoints.pack_start(*cb, Gtk::PACK_SHRINK);
	this->subMountpoints[name] = cb;

	this->scrSubmountpoints.show();
	vbSubmountpoints.show_all();
	this->lblSubmountpoints.show();
}

void GrubEnvEditorGtk::setSubmountpointSelectionState(std::string const& submountpoint, bool new_isSelected) {
	this->subMountpoints[submountpoint]->set_active(new_isSelected);
}

void GrubEnvEditorGtk::signal_partitionChanged() {
	if (!this->eventLock) {
		std::string selectedUuid = this->pChooser->getSelectedUuid();
		if (selectedUuid != "") {
			selectedUuid = "UUID=" + selectedUuid;
		}
		this->eventListener->grubEnvEditor_partitionChanged(selectedUuid);
	}
}

void GrubEnvEditorGtk::signal_bootloaderType_changed() {
	if (!this->eventLock) {
		this->eventListener->grubEnvEditor_typeChanged(this->cbType.get_active_row_number());
	}
}

void GrubEnvEditorGtk::signal_optionModified() {
	if (!this->eventLock) {
		this->eventListener->grubEnvEditor_optionModified();
	}
}

void GrubEnvEditorGtk::signal_response_action(int response_id) {
	if (response_id == Gtk::RESPONSE_CLOSE) {
		this->eventListener->grubEnvEditor_cancellationRequested();
	} else if (response_id = Gtk::RESPONSE_APPLY) {
		this->eventListener->grubEnvEditor_applyRequested();
	}
}

void GrubEnvEditorGtk::signal_submountpointToggled(Gtk::CheckButton& sender) {
	if (!eventLock) {
		if (sender.get_active()) {
			this->eventListener->submountpoint_mount_request(sender.get_label());
		} else {
			this->eventListener->submountpoint_umount_request(sender.get_label());
		}
	}
}
