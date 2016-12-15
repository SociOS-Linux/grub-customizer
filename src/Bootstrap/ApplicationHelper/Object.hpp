/*
 * Copyright (C) 2010-2014 Daniel Richter <danielrichter2007@web.de>
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

#ifndef SRC_BOOTSTRAP_APPLICATIONHELPER_OBJECT_HPP_
#define SRC_BOOTSTRAP_APPLICATIONHELPER_OBJECT_HPP_

#include "Event.hpp"
#include <map>

namespace Gc { namespace Bootstrap { namespace ApplicationHelper { class Object
{
	public: Gc::Bootstrap::ApplicationHelper::Event<Gc::Common::Exception::GenericException> onError;
	public: Gc::Bootstrap::ApplicationHelper::Event<Gc::Common::Exception::GenericException> onThreadError;

	public: Gc::Bootstrap::ApplicationHelper::Event<> onAboutDlgShowRequest;
	public: Gc::Bootstrap::ApplicationHelper::Event<Gc::Common::Type::Rule*> onEntryEditorShowRequest;
	public: Gc::Bootstrap::ApplicationHelper::Event<> onEnvEditorShowRequest;
	public: Gc::Bootstrap::ApplicationHelper::Event<> onInstallerShowRequest;
	public: Gc::Bootstrap::ApplicationHelper::Event<> onSettingsShowRequest;

	public: Gc::Bootstrap::ApplicationHelper::Event<> onListModelChange;
	public: Gc::Bootstrap::ApplicationHelper::Event<bool> onEnvChange;
	public: Gc::Bootstrap::ApplicationHelper::Event<> onListRelevantSettingChange;

	// param 1: the modified rule, param 2: whether it's a new rule
	public: Gc::Bootstrap::ApplicationHelper::Event<Gc::Common::Type::Rule*, bool> onListRuleChange;
	public: Gc::Bootstrap::ApplicationHelper::Event<> onTrashEntrySelection;
	public: Gc::Bootstrap::ApplicationHelper::Event<> onEntrySelection;
	public: Gc::Bootstrap::ApplicationHelper::Event<std::list<Gc::Common::Type::Rule*>> onEntryInsertionRequest; // TODO: do just selection - not the insertion itself
	public: Gc::Bootstrap::ApplicationHelper::Event<std::list<Gc::Common::Type::Entry*>> onEntryRemove;

	public: Gc::Bootstrap::ApplicationHelper::Event<> onInit;
	public: Gc::Bootstrap::ApplicationHelper::Event<> onSettingModelChange;
	public: Gc::Bootstrap::ApplicationHelper::Event<> onLoad; // loading finished (without preserving data)
	public: Gc::Bootstrap::ApplicationHelper::Event<> onSave;

	public: std::map<Gc::Common::Type::ViewOption, bool> viewOptions;

	public: virtual void addShutdownHandler(std::function<void ()> callback) = 0;
	public: virtual void shutdown() = 0;
	public: virtual void run() = 0;
	public: virtual ~Object(){};
};}}}



#endif /* SRC_BOOTSTRAP_APPLICATIONHELPER_OBJECT_HPP_ */
