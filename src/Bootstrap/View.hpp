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

#include "../config.hpp"
#include <memory>

class Bootstrap_View :
	public Model_DeviceDataListInterface_Connection
{
	public: std::shared_ptr<View_About> about;
	public: std::shared_ptr<View_EntryEditor> entryEditor;
	public: std::shared_ptr<View_EnvEditor> envEditor;
	public: std::shared_ptr<View_Error> error;
	public: std::shared_ptr<View_Installer> installer;
	public: std::shared_ptr<View_Main> main;
	public: std::shared_ptr<View_Settings> settings;
	public: std::shared_ptr<View_Theme> theme;
	public: std::shared_ptr<View_Trash> trash;

	public: void setDeviceDataList(Model_DeviceDataListInterface& deviceDataList);

	public: Bootstrap_View();
	public: ~Bootstrap_View();
};



#endif /* SRC_BOOTSTRAP_VIEW_HPP_ */
