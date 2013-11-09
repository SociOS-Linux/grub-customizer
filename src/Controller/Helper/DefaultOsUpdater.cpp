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

#include "DefaultOsUpdater.h"

bool Controller_Helper_DefaultOsUpdater::ruleAffectsCurrentDefaultOs(Model_Rule* rule, std::string const& currentRulePath, std::string const& currentDefaultRulePath) {
	bool result = false;

	if (rule->type == Model_Rule::SUBMENU) {
		if (currentDefaultRulePath.substr(0, currentRulePath.length() + 1) == currentRulePath + ">") {
			result = true;
		}
	} else {
		if (this->settings->getValue("GRUB_DEFAULT") == currentRulePath) {
			result = true;
		}
	}
	return result;
}

void Controller_Helper_DefaultOsUpdater::updateCurrentDefaultOs(Model_Rule* rule, std::string const& oldRulePath, std::string oldDefaultRulePath) {
	oldDefaultRulePath.replace(0, oldRulePath.length(), this->grublistCfg->getRulePath(*rule));
	this->settings->setValue("GRUB_DEFAULT", oldDefaultRulePath);
}

