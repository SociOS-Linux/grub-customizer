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

#ifndef ENTRYEDITDLG_H_
#define ENTRYEDITDLG_H_
#include <string>
#include <map>
#include <list>
#include <functional>

#include "../Common/Type.hpp"
#include "../Model/Logger/Trait/LoggerAware.hpp"
#include "../Model/Device/DeviceDataListInterface.hpp"
#include "../Model/Device/DeviceDataListInterfaceConnection.hpp"

namespace Gc { namespace View { class EntryEditor :
	public Gc::Model::Logger::Trait::LoggerAware,
	public Gc::Model::Device::DeviceDataListInterfaceConnection
{
	public: std::function<void ()> onApplyClick;
	public: std::function<void ()> onSourceModification;
	public: std::function<void ()> onOptionModification;
	public: std::function<void (std::string const& newType)> onTypeSwitch;
	public: std::function<void (std::string, std::string, std::list<std::string>)> onFileChooserSelection;
	public: std::function<void ()> onNameChange;

	public: virtual inline ~EntryEditor() {};

	public: virtual void show() = 0;
	public: virtual void setSourcecode(std::string const& source) = 0;
	public: virtual void showSourceBuildError() = 0;
	public: virtual void setApplyEnabled(bool value) = 0;
	public: virtual std::string getSourcecode() = 0;

	public: virtual void addOption(std::string const& name, std::string const& value) = 0;
	public: virtual void setOptions(std::map<std::string, std::string> options) = 0;
	public: virtual std::map<std::string, std::string> getOptions() const = 0;
	public: virtual void removeOptions() = 0;

	public: virtual void setRulePtr(Gc::Common::Type::Rule* rulePtr) = 0;
	public: virtual Gc::Common::Type::Rule* getRulePtr() = 0;

	public: virtual void hide() = 0;

	public: virtual void setAvailableEntryTypes(std::list<std::string> const& names) = 0;
	public: virtual void selectType(std::string const& name) = 0;
	public: virtual std::string getSelectedType() const = 0;
	public: virtual void setName(std::string const& name) = 0;
	public: virtual std::string getName() = 0;
	public: virtual void setNameFieldVisibility(bool visible) = 0;

	public: virtual void setErrors(std::list<std::string> const& errors) = 0;

	public: virtual void setNameIsValid(bool valid) = 0;
	public: virtual void setTypeIsValid(bool valid) = 0;
};}}

#endif /* ENTRYEDITDLG_H_ */
