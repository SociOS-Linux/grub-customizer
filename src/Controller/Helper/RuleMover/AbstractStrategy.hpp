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


#include <memory>
#include <string>

namespace Gc { namespace Controller { namespace Helper { namespace RuleMover {
class AbstractStrategy
{
	public: enum class Direction {
		DOWN = 1,
		UP = -1
	};

	protected: std::string name;

	protected: AbstractStrategy(std::string const& name)
		: name(name)
	{}

	public: virtual void move(std::shared_ptr<Gc::Model::ListCfg::Rule> rule, Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction) = 0;

	public: virtual std::string getName()
	{
		return this->name;
	}

	public: virtual ~AbstractStrategy(){};


	protected: std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>> findVisibleRules(
		std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>> ruleList,
		std::shared_ptr<Gc::Model::ListCfg::Rule> ruleAlwaysToInclude
	) {
		std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>> result;

		for (auto rule : ruleList) {
			if (rule->isVisible || rule == ruleAlwaysToInclude) {
				result.push_back(rule);
			}
		}

		return result;
	}

	protected: std::shared_ptr<Gc::Model::ListCfg::Rule> getNextRule(
		std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>> list,
		std::shared_ptr<Gc::Model::ListCfg::Rule> base,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		auto currentPosition = std::find(list.begin(), list.end(), base);

		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::UP) {
			if (currentPosition == list.begin()) {
				return nullptr;
			}
			currentPosition--;
			return *currentPosition;
		}

		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN) {
			currentPosition++; // iterator returned by end points behind to list so we have to increase before
			if (currentPosition == list.end()) {
				return nullptr;
			}
			return *currentPosition;
		}

		throw Gc::Common::Exception::LogicException("cannot handle given direction", __FILE__, __LINE__);
	}

	protected: void removeFromList(
		std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>>& list,
		std::shared_ptr<Gc::Model::ListCfg::Rule> ruleToRemove
	) {
		auto position = std::find(list.begin(), list.end(), ruleToRemove);
		list.erase(position);
	}

	protected: void insertBehind(
		std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>>& list,
		std::shared_ptr<Gc::Model::ListCfg::Rule> ruleToInsert,
		std::shared_ptr<Gc::Model::ListCfg::Rule> position,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		auto insertPosition = std::find(list.begin(), list.end(), position);
		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN) {
			insertPosition++;
		}
		list.insert(insertPosition, ruleToInsert);
	}

	protected: std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>> findProxiesWithVisibleToplevelEntries(
		std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>> proxies
	) {
		std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>> result;

		for (auto proxy : proxies) {
			for (auto rule : proxy->rules) {
				if (rule->isVisible) {
					result.push_back(proxy);
					proxy->hasVisibleRules();
					break;
				}
			}
		}

		return result;
	}

	protected: std::shared_ptr<Gc::Model::ListCfg::Proxy> getNextProxy(
		std::list<std::shared_ptr<Gc::Model::ListCfg::Proxy>> list,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> base,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		auto currentPosition = std::find(list.begin(), list.end(), base);

		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::UP) {
			if (currentPosition == list.begin()) {
				return nullptr;
			}
			currentPosition--;
			return *currentPosition;
		}

		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN) {
			currentPosition++; // iterator returned by end points behind to list so we have to increase before
			if (currentPosition == list.end()) {
				return nullptr;
			}
			return *currentPosition;
		}

		throw Gc::Common::Exception::LogicException("cannot handle given direction", __FILE__, __LINE__);
	}

	protected: std::shared_ptr<Gc::Model::ListCfg::Rule> getFirstVisibleRule(
		std::shared_ptr<Gc::Model::ListCfg::Proxy> proxy,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		auto visibleRules = this->findVisibleRules(proxy->rules, nullptr);
		if (visibleRules.size() == 0) {
			return nullptr;
		}
		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::UP) {
			return visibleRules.back();
		}

		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN) {
			return visibleRules.front();
		}

		throw Gc::Common::Exception::LogicException("cannot handle given direction", __FILE__, __LINE__);
	}

	protected: void insertIntoSubmenu(
		std::shared_ptr<Gc::Model::ListCfg::Rule>& submenu,
		std::shared_ptr<Gc::Model::ListCfg::Rule> ruleToInsert,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN) {
			submenu->subRules.push_front(ruleToInsert);
		}

		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::UP) {
			submenu->subRules.push_back(ruleToInsert);
		}
	}

	protected: unsigned int countVisibleRulesOnToplevel(std::shared_ptr<Gc::Model::ListCfg::Proxy> proxy)
	{
		unsigned int count = 0;

		for (auto rule : proxy->rules) {
			if (rule->isVisible) {
				count++;
			}
		}

		return count;
	}

	protected: Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction flipDirection(
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction in
	) {
		if (in == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::UP) {
			return Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN;
		}

		if (in == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN) {
			return Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::UP;
		}

		throw Gc::Common::Exception::LogicException("cannot handle given direction", __FILE__, __LINE__);
	}

	protected: void moveRuleToOtherProxy(
		std::shared_ptr<Gc::Model::ListCfg::Rule> ruleToMove,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> sourceProxy,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> destination,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		// replace ruleToMove by an invisible copy
		auto ruleToMoveSource = std::find(sourceProxy->rules.begin(), sourceProxy->rules.end(), ruleToMove);
		auto dummyRule = ruleToMove->clone();
		dummyRule->setVisibility(false);
		*ruleToMoveSource = dummyRule;

		this->insertIntoProxy(ruleToMove, destination, direction);
	}

	protected: void insertIntoProxy(
		std::shared_ptr<Gc::Model::ListCfg::Rule> ruleToInsert,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> destination,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		// remove old equivalent rule
		destination->removeEquivalentRules(ruleToInsert);

		// do the insertion
		auto insertPosition = destination->rules.begin();
		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::UP) {
			insertPosition = destination->rules.end();
		}

		destination->rules.insert(insertPosition, ruleToInsert);
	}

	/**
	 * extended version with replacement of old rule
	 */
	protected: void insertAsNewProxy(
		std::shared_ptr<Gc::Model::ListCfg::Rule> ruleToMove,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> proxyToCopy,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> destination,
		std::shared_ptr<Gc::Model::ListCfg::ListCfg> listCfg,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction,
		bool reAddOldRuleInvisible = true
	) {
		// replace existing rule on old proxy with invisible copy
		auto oldPos = std::find(proxyToCopy->rules.begin(), proxyToCopy->rules.end(), ruleToMove);
		auto ruleCopy = ruleToMove->clone();
		ruleCopy->setVisibility(false);
		*oldPos = ruleCopy;

		this->insertAsNewProxy(ruleToMove, proxyToCopy->dataSource, destination, listCfg, direction);
	}

	protected: void insertAsNewProxy(
		std::shared_ptr<Gc::Model::ListCfg::Rule> ruleToMove,
		std::shared_ptr<Gc::Model::ListCfg::Script> sourceScript,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> destination,
		std::shared_ptr<Gc::Model::ListCfg::ListCfg> listCfg,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		// prepare new proxy containing ruleToMove as the only visible entry
		auto newProxy = std::make_shared<Gc::Model::ListCfg::Proxy>(sourceScript, false);

		newProxy->removeEquivalentRules(ruleToMove);

		switch (direction) {
			case Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::UP:
				newProxy->rules.push_back(ruleToMove);
				break;
			case Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN:
				newProxy->rules.push_front(ruleToMove);
				break;
			default:
				throw Gc::Common::Exception::LogicException("cannot handle given direction", __FILE__, __LINE__);
		}

		// insert the new proxy
		auto insertPosition = std::find(listCfg->proxies.begin(), listCfg->proxies.end(), destination);
		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN) {
			insertPosition++;
		}

		listCfg->proxies.insert(insertPosition, newProxy);

		listCfg->renumerate();
	}
};}}}}

