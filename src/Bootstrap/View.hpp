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

#include <memory>

#include "../View/Main.hpp"
#include "../View/About.hpp"
#include "../View/Installer.hpp"
#include "../View/Trash.hpp"
#include "../View/EntryEditor.hpp"
#include "../View/Settings.hpp"
#include "../View/EnvEditor.hpp"
#include "../View/Error.hpp"
#include "../View/Theme.hpp"

#include "../Model/Device/DeviceDataListInterface.hpp"

namespace Gc { namespace Bootstrap { class View :
	public Gc::Model::Device::DeviceDataListInterfaceConnection
{
	public: std::shared_ptr<Gc::View::About> about;
	public: std::shared_ptr<Gc::View::EntryEditor> entryEditor;
	public: std::shared_ptr<Gc::View::EnvEditor> envEditor;
	public: std::shared_ptr<Gc::View::Error> error;
	public: std::shared_ptr<Gc::View::Installer> installer;
	public: std::shared_ptr<Gc::View::Main> main;
	public: std::shared_ptr<Gc::View::Settings> settings;
	public: std::shared_ptr<Gc::View::Theme> theme;
	public: std::shared_ptr<Gc::View::Trash> trash;

	public: void setDeviceDataList(Gc::Model::Device::DeviceDataListInterface& deviceDataList);

	public: View();
	public: ~View();
};}}



#endif /* SRC_BOOTSTRAP_VIEW_HPP_ */
