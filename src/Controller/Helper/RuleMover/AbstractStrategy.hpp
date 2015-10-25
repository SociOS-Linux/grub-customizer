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

#ifndef INC_Controller_Helper_RuleMover_AbstractStrategy
#define INC_Controller_Helper_RuleMover_AbstractStrategy

#include "../../../Model/Rule.hpp"
#include "../../../Model/ListCfg.hpp"
#include <memory>
#include <string>

class Controller_Helper_RuleMover_AbstractStrategy
{
	public: enum class Direction {
		DOWN = 1,
		UP = -1
	};

	protected: std::string name;

	Controller_Helper_RuleMover_AbstractStrategy(std::string const& name)
		: name(name)
	{}

	public: virtual void move(std::shared_ptr<Model_Rule> rule, Controller_Helper_RuleMover_AbstractStrategy::Direction direction) = 0;

	public: virtual std::string getName()
	{
		return this->name;
	}

	public: virtual ~Controller_Helper_RuleMover_AbstractStrategy(){};


	protected: std::list<std::shared_ptr<Model_Rule>> findVisibleRules(
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

	protected: std::shared_ptr<Model_Rule> getNextRule(
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

	protected: void removeFromList(
		std::list<std::shared_ptr<Model_Rule>>& list,
		std::shared_ptr<Model_Rule> ruleToRemove
	) {
		auto position = std::find(list.begin(), list.end(), ruleToRemove);
		list.erase(position);
	}

	protected: void insertBehind(
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
