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

#ifndef INC_Gc_Controller_Helper_RuleMover_Strategy_MoveRuleIntoSubmenu
#define INC_Gc_Controller_Helper_RuleMover_Strategy_MoveRuleIntoSubmenu

#include "../../../../Model/Rule.hpp"
#include "../../../../Model/ListCfg.hpp"
#include "../AbstractStrategy.hpp"
#include "../../../../lib/Trait/LoggerAware.hpp"
#include <memory>

namespace Gc { namespace Controller { namespace Helper { namespace RuleMover { namespace Strategy {
class MoveRuleIntoSubmenu :
	public Gc::Controller::Helper::RuleMover::AbstractStrategy,
	public Model_ListCfg_Connection,
	public Trait_LoggerAware
{
	public: MoveRuleIntoSubmenu()
		: Gc::Controller::Helper::RuleMover::AbstractStrategy("MoveRuleIntoSubmenu")
	{}

	public: void move(std::shared_ptr<Model_Rule> rule, Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction)
	{
		auto proxy = this->grublistCfg->proxies.getProxyByRule(rule);
		auto& ruleList = proxy->getRuleList(proxy->getParentRule(rule));

		auto visibleRules = this->findVisibleRules(ruleList, rule);

		auto nextRule = this->getNextRule(visibleRules, rule, direction);

		if (nextRule == nullptr) {
			throw Gc::Controller::Helper::RuleMover::MoveFailedException("next rule not found", __FILE__, __LINE__);
		}

		if (nextRule->type != Model_Rule::SUBMENU) {
			throw Gc::Controller::Helper::RuleMover::MoveFailedException("next rule is not a submenu", __FILE__, __LINE__);
		}

		this->removeFromList(ruleList, rule);
		this->insertIntoSubmenu(nextRule, rule, direction);
	}
};}}}}}
#endif
