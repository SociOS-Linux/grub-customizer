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

#ifndef SETTINGSCONTROLLER_H_
#define SETTINGSCONTROLLER_H_

#include "../Controller/ThreadController.hpp"

class SettingsController {
public:
	virtual inline ~SettingsController(){};
	virtual void loadResolutionsAction() = 0;
	virtual void updateSettingsDataAction() = 0;
	virtual void updateResolutionlistAction() = 0;

	virtual void hideAction() = 0;
	virtual void showAction(bool burgMode) = 0;
	virtual void syncAction() = 0;
};


#endif /* SETTINGSCONTROLLER_H_ */
