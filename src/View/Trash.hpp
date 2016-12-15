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

#ifndef ENTRYADDDLG_H_
#define ENTRYADDDLG_H_

#include <list>
#include <string>
#include <functional>

/**
 * Interface to be implemented by dialogs which lets the user adding scripts
 */
namespace Gc { namespace View { class Trash :
	public Gc::Model::Logger::Trait::LoggerAware
{
	public: virtual inline ~Trash() {};

	public: std::function<void ()> onRestore;
	public: std::function<void ()> onDeleteClick;
	public: std::function<void (std::list<Gc::Common::Type::Rule*> const& selectedEntries)> onSelectionChange;

	//clear the script menu
	public: virtual void clear()=0;
	//gets the index of the selected script item
	public: virtual std::list<Gc::Common::Type::Rule*> getSelectedEntries()=0;
	//adds a new item
	public: virtual void addItem(Gc::View::Model::ListItem<Gc::Common::Type::Rule, Gc::Common::Type::Script> const& listItem)=0;
	//whether to active the delete button
	public: virtual void setDeleteButtonEnabled(bool val) = 0;
	//show this dialog
	public: virtual void show()=0;
	//thise this dialog
	public: virtual void hide()=0;

	public: virtual void askForDeletion(std::list<std::string> const& names) = 0;

	public: virtual void setOptions(std::map<Gc::Common::Type::ViewOption, bool> const& viewOptions) = 0;

	public: virtual void selectEntries(std::list<Gc::Common::Type::Rule*> const& entries) = 0;

	public: virtual void setRestoreButtonSensitivity(bool sensitivity) = 0;

	public: virtual void setDeleteButtonVisibility(bool visibility) = 0;
};}}

#endif
