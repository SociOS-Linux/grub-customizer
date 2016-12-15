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

#ifndef INC_Gc_Controller_Helper_RuleMover_Strategy_MoveRuleOnSameLevelInsideProxy
#define INC_Gc_Controller_Helper_RuleMover_Strategy_MoveRuleOnSameLevelInsideProxy

#include "../../../../Model/ListCfg/Rule.hpp"
#include "../../../../Model/ListCfg/ListCfg.hpp"
#include "../AbstractStrategy.hpp"
#include "../../../../Model/Logger/Trait/LoggerAware.hpp"
#include <memory>

namespace Gc { namespace Controller { namespace Helper { namespace RuleMover { namespace Strategy {
class MoveRuleOnSameLevelInsideProxy :
	public Gc::Controller::Helper::RuleMover::AbstractStrategy,
	public Gc::Model::ListCfg::ListCfgConnection,
	public Gc::Model::Logger::Trait::LoggerAware
{
	public: MoveRuleOnSameLevelInsideProxy()
		: Gc::Controller::Helper::RuleMover::AbstractStrategy("MoveRuleOnSameLevelInsideProxy")
	{}

	public: void move(std::shared_ptr<Gc::Model::ListCfg::Rule> rule, Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction)
	{
		auto proxy = this->grublistCfg->proxies.getProxyByRule(rule);
		auto& ruleList = proxy->getRuleList(proxy->getParentRule(rule));

		auto visibleRules = this->findVisibleRules(ruleList, rule);

		auto nextRule = this->getNextRule(visibleRules, rule, direction);
		if (nextRule == nullptr) {
			throw Gc::Controller::Helper::RuleMover::MoveFailedException("no next rule found", __FILE__, __LINE__);
		}

		this->removeFromList(ruleList, rule);
		this->insertBehind(ruleList, rule, nextRule, direction);
	}
};}}}}}
#endif
