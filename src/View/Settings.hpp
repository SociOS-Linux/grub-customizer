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

#ifndef SETTINGSDLG_H_
#define SETTINGSDLG_H_
#include <functional>

#include "../Model/Logger/Trait/LoggerAware.hpp"

/**
 * Interface to be implemented by settings dialogs
 */
namespace Gc { namespace View { class Settings :
	public Gc::Model::Logger::Trait::LoggerAware
{
	public: std::function<void ()> onDefaultSystemChange;
	public: std::function<void (std::string const& name)> onCustomSettingChange;
	public: std::function<void ()> onAddCustomSettingClick;
	public: std::function<void (std::string const& name)> onRemoveCustomSettingClick;
	public: std::function<void ()> onShowMenuSettingChange;
	public: std::function<void ()> onOsProberSettingChange;
	public: std::function<void ()> onTimeoutSettingChange;
	public: std::function<void ()> onKernelParamsChange;
	public: std::function<void ()> onRecoverySettingChange;
	public: std::function<void ()> onCustomResolutionChange;
	public: std::function<void ()> onUseCustomResolutionChange;
	public: std::function<void ()> onHide;

	public: virtual inline ~Settings() {};

	public: enum class DefEntryType {
		PREDEFINED,
		SAVED
	};
	public: class CustomOption {
		public: std::string name, old_name, value;
		public: bool isActive;
	};
	//show this dialog
	public: virtual void show()=0;
	//hide this dialog
	public: virtual void hide()=0;
	//reads the selection row from the custom options list
	public: virtual std::string getSelectedCustomOption()=0;
	//adds an entry to the end of the default entry chooser
	public: virtual void addEntryToDefaultEntryChooser(std::string const& value, std::string const& label)=0;
	//removes all items from the default entry chooser
	public: virtual void clearDefaultEntryChooser()=0;
	//removes all item from the resolution chooser
	public: virtual void clearResolutionChooser()=0;
	//adds an item to the end of the resolution chooser
	public: virtual void addResolution(std::string const& resolution)=0;
	//gets the name of the default menu entry
	public: virtual std::string getSelectedDefaultGrubValue()=0;
	//adds an option the the generic setting list
	public: virtual void addCustomOption(bool isActive, std::string const& name, std::string const& value)=0;
	//select the specified custom option entry
	public: virtual void selectCustomOption(std::string const& name)=0;
	//removes all generic setting rows
	public: virtual void removeAllSettingRows()=0;
	//reads the given generic option
	public: virtual CustomOption getCustomOption(std::string const& name)=0;
	//sets which type of default entry to use
	public: virtual void setActiveDefEntryOption(DefEntryType option)=0;
	//determines which type of default entry should be used
	public: virtual DefEntryType getActiveDefEntryOption()=0;
	//sets the default entry
	public: virtual void setDefEntry(std::string const& defEntry)=0;
	//sets whether the show menu checkbox should be active or not
	public: virtual void setShowMenuCheckboxState(bool isActive)=0;
	//determines whether the show menu checkbox is active or not
	public: virtual bool getShowMenuCheckboxState()=0;
	//sets whether the os-prober checkbox should be active or not
	public: virtual void setOsProberCheckboxState(bool isActive)=0;
	//sets whether the os-prober checkbox is active or not
	public: virtual bool getOsProberCheckboxState()=0;
	//shows the information about a conflict between os-prober and hidden menus
	public: virtual void showHiddenMenuOsProberConflictMessage()=0;
	//sets the grub menu timeout
	public: virtual void setTimeoutValue(int value)=0;
	//sets weather the grub menu timeout is active
	public: virtual void setTimeoutActive(bool active)=0;
	//reads the grub menu timeout
	public: virtual int getTimeoutValue()=0;
	//reads the grub menu timeout as string
	public: virtual std::string getTimeoutValueString()=0;
	//says wheather the timeout checkbox is activates
	public: virtual bool getTimeoutActive()=0;
	//sets kernel params
	public: virtual void setKernelParams(std::string const& params)=0;
	//reads kernel params
	public: virtual std::string getKernelParams()=0;
	//sets whether the recovery checkbox should be active or not
	public: virtual void setRecoveryCheckboxState(bool isActive)=0;
	//determines whether the recovery checkbox is active or not
	public: virtual bool getRecoveryCheckboxState()=0;
	//sets whether the resolution should be active or not
	public: virtual void setResolutionCheckboxState(bool isActive)=0;
	//determines whether the resolution is active or not
	public: virtual bool getResolutionCheckboxState()=0;
	//sets the selected resolution
	public: virtual void setResolution(std::string const& resolution)=0;
	//reads the selected resolution
	public: virtual std::string getResolution()=0;
};}}

#endif
