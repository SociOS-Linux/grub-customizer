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

#ifndef INC_Controller_Helper_RuleMover_Strategy_MoveRuleOnSameLevelInsideProxy
#define INC_Controller_Helper_RuleMover_Strategy_MoveRuleOnSameLevelInsideProxy

#include "../../../../Model/Rule.hpp"
#include "../../../../Model/ListCfg.hpp"
#include "../AbstractStrategy.hpp"
#include <memory>

class Controller_Helper_RuleMover_Strategy_MoveRuleOnSameLevelInsideProxy :
	public Controller_Helper_RuleMover_AbstractStrategy,
	public Model_ListCfg_Connection
{
	void move(std::shared_ptr<Model_Rule> rule, Controller_Helper_RuleMover_AbstractStrategy::Direction direction)
	{
		auto proxy = this->grublistCfg->proxies.getProxyByRule(rule);
		auto& ruleList = proxy->getRuleList(proxy->getParentRule(rule));

		auto visibleRules = this->findVisibleRules(ruleList, rule);

		auto nextRule = this->getNextRule(visibleRules, rule, direction);
		if (nextRule == nullptr) {
			throw Controller_Helper_RuleMover_MoveFailedException("no next rule found", __FILE__, __LINE__);
		}

		this->removeFromList(ruleList, rule);
		this->insertBehind(ruleList, rule, nextRule, direction);
	}

	private: std::list<std::shared_ptr<Model_Rule>> findVisibleRules(
		std::list<std::shared_ptr<Model_Rule>> ruleList,
		std::shared_ptr<Model_Rule> ruleAlwaysToInclude
	) {
		std::list<std::shared_ptr<Model_Rule>> result;

		for (auto rule : ruleList) {
			if (rule->isVisible || rule == ruleAlwaysToInclude) {
				result.push_back(rule);
			}
		}

		return result;
	}

	private: std::shared_ptr<Model_Rule> getNextRule(
		std::list<std::shared_ptr<Model_Rule>> list,
		std::shared_ptr<Model_Rule> base,
		Controller_Helper_RuleMover_AbstractStrategy::Direction direction
	) {
		auto currentPosition = std::find(list.begin(), list.end(), base);

		if (direction == Controller_Helper_RuleMover_AbstractStrategy::Direction::UP) {
			if (currentPosition == list.begin()) {
				return nullptr;
			}
			currentPosition--;
			return *currentPosition;
		}

		if (direction == Controller_Helper_RuleMover_AbstractStrategy::Direction::DOWN) {
			currentPosition++; // iterator returned by end points behind to list so we have to increase before
			if (currentPosition == list.end()) {
				return nullptr;
			}
			return *currentPosition;
		}

		throw LogicException("cannot handle given direction", __FILE__, __LINE__);
	}

	private: void removeFromList(
		std::list<std::shared_ptr<Model_Rule>>& list,
		std::shared_ptr<Model_Rule> ruleToRemove
	) {
		auto position = std::find(list.begin(), list.end(), ruleToRemove);
		list.erase(position);
	}

	private: void insertBehind(
		std::list<std::shared_ptr<Model_Rule>>& list,
		std::shared_ptr<Model_Rule> ruleToInsert,
		std::shared_ptr<Model_Rule> position,
		Controller_Helper_RuleMover_AbstractStrategy::Direction direction
	) {
		auto insertPosition = std::find(list.begin(), list.end(), position);
		if (direction == Controller_Helper_RuleMover_AbstractStrategy::Direction::DOWN) {
			insertPosition++;
		}
		list.insert(insertPosition, ruleToInsert);
	}
};
#endif
