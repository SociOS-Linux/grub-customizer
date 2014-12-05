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
#include "../View/Gtk/About.hpp"
#include "../View/Gtk/EntryEditor.hpp"
#include "../View/Gtk/EnvEditor.hpp"
#include "../View/Gtk/Error.hpp"
#include "../View/Gtk/Installer.hpp"
#include "../View/Gtk/Main.hpp"
#include "../View/Gtk/Settings.hpp"
#include "../View/Gtk/Theme.hpp"
#include "../View/Gtk/Trash.hpp"
#include "View.hpp"

Bootstrap_View::Bootstrap_View(int argc, char** argv)
	: about(NULL)
	, entryEditor(NULL)
	, envEditor(NULL)
	, error(NULL)
	, installer(NULL)
	, main(NULL)
	, settings(NULL)
	, theme(NULL)
	, trash(NULL)
{
	new Gtk::Main(argc, argv); // stored at Gtk::Main::instance and deleted in destructor of this class

	this->about = new View_Gtk_About;
	this->entryEditor = new View_Gtk_EntryEditor;
	this->envEditor = new View_Gtk_EnvEditor;
	this->error = new View_Gtk_Error;
	this->installer = new View_Gtk_Installer;
	this->main = new View_Gtk_Main;
	this->settings = new View_Gtk_Settings;
	this->theme = new View_Gtk_Theme;
	this->trash = new View_Gtk_Trash;

	View_Gtk_Main* main         = &dynamic_cast<View_Gtk_Main&>(*this->main);
	View_Gtk_Settings* settings = &dynamic_cast<View_Gtk_Settings&>(*this->settings);
	View_Gtk_Trash* trash       = &dynamic_cast<View_Gtk_Trash&>(*this->trash);
	View_Gtk_Theme* theme       = &dynamic_cast<View_Gtk_Theme&>(*this->theme);

	main->putSettingsDialog(settings->getCommonSettingsPane(), settings->getAppearanceSettingsPane());
	main->putTrashList(trash->getList());
	settings->putThemeSelector(theme->getThemeSelector());
	settings->putThemeEditArea(theme->getEditorBox());
}

Bootstrap_View::~Bootstrap_View() {
	delete about;
	delete entryEditor;
	delete envEditor;
	delete error;
	delete installer;
	delete main;
	delete settings;
	delete theme;
	delete trash;

	delete Gtk::Main::instance();
}

void Bootstrap_View::run() {
	Gtk::Main::run();
}
