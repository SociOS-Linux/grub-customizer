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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SETTINGSDLG_H_
#define SETTINGSDLG_H_

#include "ColorChooser.h"
#include "../interface/evt_settings.h"
/**
 * Interface to be implemented by settings dialogs
 */
class SettingsDlg {
public:
	virtual inline ~SettingsDlg() {};

	enum DefEntryType {
		DEF_ENTRY_PREDEFINED,
		DEF_ENTRY_SAVED
	};
	enum ColorChooserType {
		COLOR_CHOOSER_DEFAULT_FONT,
		COLOR_CHOOSER_DEFAULT_BACKGROUND,
		COLOR_CHOOSER_HIGHLIGHT_FONT,
		COLOR_CHOOSER_HIGHLIGHT_BACKGROUND
	};
	enum Exception {
		REQUESTED_CUSTOM_OPTION_NOT_FOUND
	};
	struct CustomOption {
		std::string name, old_name, value;
		bool isActive;
	};
	//assigns the event listener
	virtual void setEventListener(EventListener_settings& eventListener)=0;
	//show this dialog
	virtual void show(bool burgMode)=0;
	//hide this dialog
	virtual void hide()=0;
	//returns an interface to the given color chooser
	virtual ColorChooser& getColorChooser(ColorChooserType type)=0;
	//get the name of the selected font
	virtual std::string getFontName()=0;
	//get the font size
	virtual int getFontSize()=0;
	//set the name of the selected font
	virtual void setFontName(std::string const& value)=0;
	//reads the selection row from the custom options list
	virtual std::string getSelectedCustomOption()=0;
	//adds an entry to the end of the default entry chooser
	virtual void addEntryToDefaultEntryChooser(std::string const& labelPathValue, std::string const& labelPathLabel, std::string const& numericPathValue, std::string const& numericPathLabel)=0;
	//set the entry titles to be viewed in the preview image
	virtual void setPreviewEntryTitles(std::list<std::string> const& entries)=0;
	//removes all items from the default entry chooser
	virtual void clearDefaultEntryChooser()=0;
	//removes all item from the resolution chooser
	virtual void clearResolutionChooser()=0;
	//adds an item to the end of the resolution chooser
	virtual void addResolution(std::string const& resolution)=0;
	//gets the name of the default menu entry
	virtual std::string getSelectedDefaultGrubValue()=0;
	//adds an option the the generic setting list
	virtual void addCustomOption(bool isActive, std::string const& name, std::string const& value)=0;
	//select the specified custom option entry
	virtual void selectCustomOption(std::string const& name)=0;
	//removes all generic setting rows
	virtual void removeAllSettingRows()=0;
	//reads the given generic option
	virtual CustomOption getCustomOption(std::string const& name)=0;
	//sets which type of default entry to use
	virtual void setActiveDefEntryOption(DefEntryType option)=0;
	//determines which type of default entry should be used
	virtual DefEntryType getActiveDefEntryOption()=0;
	//sets the default entry
	virtual void setDefEntry(std::string const& defEntry)=0;
	//sets whether the show menu checkbox should be active or not
	virtual void setShowMenuCheckboxState(bool isActive)=0;
	//determines whether the show menu checkbox is active or not
	virtual bool getShowMenuCheckboxState()=0;
	//sets whether the os-prober checkbox should be active or not
	virtual void setOsProberCheckboxState(bool isActive)=0;
	//sets whether the os-prober checkbox is active or not
	virtual bool getOsProberCheckboxState()=0;
	//shows the information about a conflict between os-prober and hidden menus
	virtual void showHiddenMenuOsProberConflictMessage()=0;
	//sets the grub menu timeout
	virtual void setTimeoutValue(int value)=0;
	//reads the grub menu timeout
	virtual int getTimeoutValue()=0;
	//reads the grub menu timeout as string
	virtual std::string getTimeoutValueString()=0;
	//sets kernel params
	virtual void setKernelParams(std::string const& params)=0;
	//reads kernel params
	virtual std::string getKernelParams()=0;
	//sets whether the recovery checkbox should be active or not
	virtual void setRecoveryCheckboxState(bool isActive)=0;
	//determines whether the recovery checkbox is active or not
	virtual bool getRecoveryCheckboxState()=0;
	//sets whether the resolution should be active or not
	virtual void setResolutionCheckboxState(bool isActive)=0;
	//determines whether the resolution is active or not
	virtual bool getResolutionCheckboxState()=0;
	//sets the selected resolution
	virtual void setResolution(std::string const& resolution)=0;
	//reads the selected resolution
	virtual std::string getResolution()=0;
	//sets the background image, remove it, if empty string is given
	virtual void setBackgroundImagePreviewPath(std::string const& menuPicturePath, bool isInGrubDir)=0;
	//reads the selected background image path
	virtual std::string getBackgroundImagePath()=0;
};

#endif
