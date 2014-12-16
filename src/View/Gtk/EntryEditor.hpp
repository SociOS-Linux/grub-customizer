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

#ifndef ENTRYEDITDLGGTK_H_
#define ENTRYEDITDLGGTK_H_
#include "../EntryEditor.hpp"

#include "../../lib/Helper.hpp"
#include <libintl.h>
#include <gtkmm.h>

#include "Element/PartitionChooser.hpp"

class View_Gtk_EntryEditor :
	public View_EntryEditor,
	public Gtk::Dialog
{
	private: Gtk::Notebook tabbox;
	private: Gtk::TextView tvSource;
	private: Gtk::ScrolledWindow scrSource;
	private: Gtk::ScrolledWindow scrOptions;
	private: Gtk::Table tblOptions;
	private: std::map<std::string, Gtk::Widget*> optionMap;
	private: std::map<std::string, Gtk::Label*> labelMap;
	private: Gtk::ComboBoxText cbType;
	private: Gtk::Label lblType;
	private: bool lock_state;

	private: Rule* rulePtr = nullptr;

	private: virtual std::string mapOptionName(std::string const& name)
	{
		if (name == "partition_uuid")
			return gettext("_Partition");
		else if (name == "initramfs")
			return gettext("_Initial ramdisk");
		else if (name == "linux_image")
			return gettext("_Linux image");
		else if (name == "memtest_image")
			return gettext("_Memtest image");
		else if (name == "iso_path")
			return gettext("Path to iso file");
		else if (name == "locale")
			return gettext("Locale");
		else if (name == "other_params")
			return gettext("Kernel params");
		else
			return name;
	}

	public:	View_Gtk_EntryEditor() :
		lblType(gettext("_Type:"), true),
		lock_state(false)
	{
		this->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		this->add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

		this->set_default_size(500, 400);
		this->set_title(Glib::ustring() + gettext("Entry editor") + " - Grub Customizer");
		this->set_icon_name("grub-customizer");

		Gtk::Box& vbMain = *this->get_vbox();
		vbMain.pack_start(this->tabbox);

		tabbox.append_page(this->scrOptions, gettext("Options"));
		tabbox.append_page(this->scrSource, gettext("Source"));
		scrOptions.add(this->tblOptions);
		scrOptions.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrSource.add(this->tvSource);
		scrSource.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

		this->tblOptions.attach(this->lblType, 0, 1, 0, 1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK, 5, 5);
		this->tblOptions.attach(this->cbType, 1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 5, 5);
		lblType.set_mnemonic_widget(cbType);
		lblType.set_alignment(Pango::ALIGN_RIGHT);

		this->signal_response().connect(sigc::mem_fun(this, &View_Gtk_EntryEditor::signal_response_action));
		this->cbType.signal_changed().connect(sigc::mem_fun(this, &View_Gtk_EntryEditor::signal_typeModified));

		this->tvSource.signal_key_release_event().connect(sigc::mem_fun(this, &View_Gtk_EntryEditor::signal_sourceModified));
	}

	public:	void setSourcecode(std::string const& source)
	{
		std::string optimizedSource = Helper::str_replace("\n\t", "\n", source);
		if (optimizedSource[0] == '\t') {
			optimizedSource = optimizedSource.substr(1);
		}
		this->tvSource.get_buffer()->set_text(optimizedSource);
	}

	public:	std::string getSourcecode()
	{
		std::string optimizedSourcecode = this->tvSource.get_buffer()->get_text();
		std::string withIndent = Helper::str_replace("\n", "\n\t", optimizedSourcecode);
		if (withIndent.size() >= 2 && withIndent.substr(withIndent.size() - 2) == "\n\t") {
			withIndent.replace(withIndent.size() - 2, 2, "\n");
		} else if (withIndent.size() >= 1 && withIndent[withIndent.size() - 1] != '\n') {
			withIndent += '\n'; // add trailing slash
		}
		return "\t" + withIndent;
	}

	public:	void addOption(std::string const& name, std::string const& value)
	{
		int pos = name == "partition_uuid" ? 1 : this->optionMap.size() + 2; // partition should be the first option
		Gtk::Label* label = Gtk::manage(new Gtk::Label(this->mapOptionName(name) + ":", true));
		label->set_alignment(Pango::ALIGN_RIGHT);
		this->tblOptions.attach(*label, 0, 1, pos, pos+1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK, 5, 5);
		this->labelMap[name] = label;

		Gtk::Widget* addedWidget = nullptr;
		if (name != "partition_uuid" || this->deviceDataList == nullptr) {
			Gtk::Entry* entry = Gtk::manage(new Gtk::Entry());
			entry->set_text(value);
			entry->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_EntryEditor::signal_optionsModified));
			addedWidget = entry;
		} else {
			View_Gtk_Element_PartitionChooser* pChooserDD = Gtk::manage(new View_Gtk_Element_PartitionChooser(value, *this->deviceDataList));
			pChooserDD->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_EntryEditor::signal_optionsModified));
			addedWidget = pChooserDD;
		}

		this->tblOptions.attach(*addedWidget, 1, 2, pos, pos+1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK, 5, 5);
		label->set_mnemonic_widget(*addedWidget);

		this->optionMap[name] = addedWidget;
		if (this->get_visible()) {
			this->tblOptions.show_all();
		}
	}

	public:	void setOptions(std::map<std::string, std::string> options)
	{
		this->removeOptions();
		for (std::map<std::string, std::string>::iterator iter = options.begin(); iter != options.end(); iter++) {
			this->addOption(iter->first, iter->second);
		}
	}

	public:	std::map<std::string, std::string> getOptions() const
	{
		std::map<std::string, std::string> result;
		for (std::map<std::string, Gtk::Widget*>::const_iterator iter = this->optionMap.begin(); iter != this->optionMap.end(); iter++) {
			try {
				result[iter->first] = dynamic_cast<View_Gtk_Element_PartitionChooser&>(*iter->second).getSelectedUuid();
			} catch (std::bad_cast const& e) {
				result[iter->first] = dynamic_cast<Gtk::Entry&>(*iter->second).get_text();
			}
		}
		return result;
	}

	public:	void removeOptions()
	{
		for (std::map<std::string, Gtk::Label*>::iterator iter = this->labelMap.begin(); iter != this->labelMap.end(); iter++) {
			this->tblOptions.remove(*iter->second);
		}
		this->labelMap.clear();
		for (std::map<std::string, Gtk::Widget*>::iterator iter = this->optionMap.begin(); iter != this->optionMap.end(); iter++) {
			this->tblOptions.remove(*iter->second);
		}
		this->optionMap.clear();
	}

	public:	void setRulePtr(Rule* rulePtr)
	{
		this->rulePtr = rulePtr;
	}

	public:	Rule* getRulePtr()
	{
		return this->rulePtr;
	}

	public:	void show()
	{
		Gtk::Window::show_all();
	}

	public:	void hide()
	{
		Gtk::Window::hide();
	}

	public:	void setAvailableEntryTypes(std::list<std::string> const& names)
	{
		this->cbType.remove_all();
		for (std::list<std::string>::const_iterator iter = names.begin(); iter != names.end(); iter++) {
			this->cbType.append(*iter);
		}
		this->cbType.append(gettext("Other"));
	}

	public:	void selectType(std::string const& name)
	{
		std::string name2 = name;
		if (name2 == "") {
			name2 = gettext("Other");
		}

		this->lock_state = true;
		this->cbType.set_active_text(name2);
		this->lock_state = false;
	}

	public:	std::string getSelectedType() const
	{
		return this->cbType.get_active_text();
	}

	private: void signal_response_action(int response_id)
	{
		if (response_id == Gtk::RESPONSE_OK){
			this->onApplyClick();
		}
		this->hide();
	}

	private: bool signal_sourceModified(GdkEventKey* event)
	{
		if (!this->lock_state) {
			this->onSourceModification();
		}
		return true;
	}

	private: void signal_optionsModified() {
		if (!this->lock_state) {
			this->onOptionModification();
		}
	}

	private: void signal_typeModified() {
		if (!this->lock_state) {
			this->onTypeSwitch(this->cbType.get_active_text() != gettext("Other") ? this->cbType.get_active_text() : "");
		}
	}
};

#endif /* ENTRYEDITDLGGTK_H_ */
