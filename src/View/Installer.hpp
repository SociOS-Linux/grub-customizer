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

#ifndef GRUBINSTALLDLG_H_
#define GRUBINSTALLDLG_H_
#include <functional>

#include "../Model/Logger/Trait/LoggerAware.hpp"

/**
 * Interface for dialogs which helps users to install grub into the MBR
 */
namespace Gc { namespace View { class Installer :
	public Gc::Model::Logger::Trait::LoggerAware
{
	public: std::function<void (std::string device)> onInstallClick;

	public: virtual inline ~Installer() {};

	//show this dialog
	public: virtual void show() = 0;
	//show the information that grub has been installed completely
	public: virtual void showMessageGrubInstallCompleted(std::string const& msg) = 0;
};}}

#endif
