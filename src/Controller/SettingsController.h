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

#ifndef SETTINGSCONTROLLER_H_
#define SETTINGSCONTROLLER_H_

class SettingsController {
public:
	virtual inline ~SettingsController(){};
	virtual void updateSettingsDataAction() = 0;
	virtual void updateResolutionlistAction() = 0;
	virtual void updateDefaultSystemAction() = 0;
	virtual void updateCustomSettingAction(std::string const& name) = 0;
	virtual void addCustomSettingAction() = 0;
	virtual void removeCustomSettingAction(std::string const& name) = 0;
	virtual void updateShowMenuSettingAction() = 0;
	virtual void updateOsProberSettingAction() = 0;
	virtual void updateTimeoutSettingAction() = 0;
	virtual void updateKernelParamsAction() = 0;
	virtual void updateRecoverySettingAction() = 0;
	virtual void updateCustomResolutionAction() = 0;
	virtual void updateColorSettingsAction() = 0;
	virtual void updateFontSettingsAction(bool removeFont) = 0;
	virtual void updateBackgroundImageAction() = 0;
	virtual void updateUseCustomResolutionAction() = 0;
	virtual void removeBackgroundImageAction() = 0;
	virtual void hideAction() = 0;
	virtual void showAction(bool burgMode) = 0;
	virtual void syncAction() = 0;
};


#endif /* SETTINGSCONTROLLER_H_ */
