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

#ifndef SRC_BOOTSTRAP_VIEW_HPP_
#define SRC_BOOTSTRAP_VIEW_HPP_

#include "../View/Main.hpp"
#include "../View/About.hpp"
#include "../View/Installer.hpp"
#include "../View/Trash.hpp"
#include "../View/EntryEditor.hpp"
#include "../View/Settings.hpp"
#include "../View/EnvEditor.hpp"
#include "../View/Error.hpp"
#include "../View/Theme.hpp"

class Bootstrap_View
{
	public: View_About* about;
	public: View_EntryEditor* entryEditor;
	public: View_EnvEditor* envEditor;
	public: View_Error* error;
	public: View_Installer* installer;
	public: View_Main* main;
	public: View_Settings* settings;
	public: View_Theme* theme;
	public: View_Trash* trash;

	public: Bootstrap_View(int argc, char** argv);
	public: ~Bootstrap_View();

	public: void run();
};



#endif /* SRC_BOOTSTRAP_VIEW_HPP_ */
