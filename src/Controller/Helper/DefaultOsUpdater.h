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

#ifndef DEFAULTOSUPDATER_H_
#define DEFAULTOSUPDATER_H_

#include "../../Model/ListCfg.h"
#include "../../Model/SettingsManagerData.h"

class Controller_Helper_DefaultOsUpdater :
	public Model_SettingsManagerData_Connection,
	public Model_ListCfg_Connection {
public:
	bool ruleAffectsCurrentDefaultOs(Model_Rule* rule, std::string const& currentRulePath, std::string const& currentDefaultRulePath);
	void updateCurrentDefaultOs(Model_Rule* rule, std::string const& currentRulePath, std::string currentDefaultRulePath);
};


#endif /* DEFAULTOSUPDATER_H_ */
