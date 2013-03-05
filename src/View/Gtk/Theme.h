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

#ifndef THEME_GTK_H_
#define THEME_GTK_H_

#include "../Theme.h"
#include <gtkmm.h>
#include <string>
#include <libintl.h>
#include "../../Controller/ThemeController.h"
#include "../../lib/CommonClass.h"

class View_Gtk_Theme : public View_Theme, public Gtk::Dialog, public CommonClass {
	Gtk::HPaned hpMain;
	Gtk::Toolbar toolbar;
	Gtk::ToolButton tbttAdd, tbttRemove;

	Gtk::HBox hbTheme;
	Gtk::Label lblTheme;
	Gtk::ComboBoxText cbTheme;

	Gtk::VBox vbFiles;
	Gtk::ScrolledWindow scrFiles;
	Gtk::ListViewText lvFiles;
	Gtk::VBox vbEdit, vbEditInner;
	Gtk::ScrolledWindow scrEdit;
	Gtk::TextView txtEdit;
	Gtk::Image imgPreview;
	Gtk::HBox hbFileSelection;
	Gtk::Label lblFileSelection;
	Gtk::FileChooserButton fcFileSelection;
	Glib::RefPtr<Gtk::SizeGroup> sizeGroupFooter;

	ThemeController* eventListener;
	bool event_lock;
	Glib::ustring _selectedFileName;
	std::string _getSelectedFileName();
public:
	View_Gtk_Theme();
	void setEventListener(ThemeController& eventListener);

	void addFile(std::string const& fileName);
	void clear();
	void setText(std::string const& text);
	void setImage(std::string const& path);
	void selectFile(std::string const& fileName, bool startEdit = false);
	void show();
private:
	void signal_fileAddClick();
	void signal_fileRemoveClick();
	void signal_fileSelected();
	void signal_fileRenamed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
	void signal_fileChosen();
	void signal_textChanged();
};


#endif /* THEME_GTK_H_ */
