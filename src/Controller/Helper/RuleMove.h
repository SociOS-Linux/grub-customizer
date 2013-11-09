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

#ifndef RULEMOVE_H_
#define RULEMOVE_H_
#include "../../lib/assert.h"
#include "../../View/Trait/ViewAware.h"
#include "../../View/Main.h"
#include "../../Model/ListCfg.h"
#include "../../Controller/Trait/ControllerAware.h"
#include "../../Controller/MainController.h"
#include "../../Model/Env.h"
#include "../../Model/SettingsManagerData.h"
#include "DefaultOsUpdater.h"

class Controller_Helper_RuleMove :
	public View_Trait_ViewAware<View_Main>,
	public Model_ListCfg_Connection,
	public Trait_ControllerAware<MainController>,
	public Model_Env_Connection,
	public Model_SettingsManagerData_Connection {
private:
	Controller_Helper_DefaultOsUpdater defaultOsUpdater;

	std::list<Rule*> _populateSelection(std::list<Rule*> rules);
	void _populateSelection(std::list<Rule*>& rules, Model_Rule* currentRule, int direction, bool checkScript);
	int _countRulesUntilNextRealRule(Model_Rule* baseRule, int direction);
	std::list<Rule*> _removePlaceholdersFromSelection(std::list<Rule*> rules);
public:
	void setDefaultOsUpdater(Controller_Helper_DefaultOsUpdater defaultOsUpdater);
	void move(std::list<Rule*> rules, int direction);
};


#endif /* RULEMOVE_H_ */
