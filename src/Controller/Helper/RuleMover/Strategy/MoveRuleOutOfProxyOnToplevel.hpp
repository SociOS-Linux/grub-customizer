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

#ifndef INC_Controller_Helper_RuleMover_Strategy_MoveRuleOutOfProxyOnToplevel
#define INC_Controller_Helper_RuleMover_Strategy_MoveRuleOutOfProxyOnToplevel

#include "../../../../Model/Rule.hpp"
#include "../../../../Model/ListCfg.hpp"
#include "../AbstractStrategy.hpp"
#include "../../../../lib/Trait/LoggerAware.hpp"
#include <memory>
#include <bitset>
#include <set>
#include <unordered_map>

class Controller_Helper_RuleMover_Strategy_MoveRuleOutOfProxyOnToplevel :
	public Controller_Helper_RuleMover_AbstractStrategy,
	public Model_ListCfg_Connection,
	public Trait_LoggerAware
{
	private: enum Task
	{
		MoveOwnProxy,
		MoveOwnEntry,
		MoveForeignEntry,
		SplitOwnProxy,
		SplitForeignProxy,
		DeleteOwnProxy,
		DeleteForeignProxy
	};

	public: void move(std::shared_ptr<Model_Rule> rule, Controller_Helper_RuleMover_AbstractStrategy::Direction direction)
	{
		auto proxy = this->grublistCfg->proxies.getProxyByRule(rule);
		auto proxiesWithVisibleEntries = this->findProxiesWithVisibleToplevelEntries();

		auto nextProxy = this->getNextProxy(proxiesWithVisibleEntries, proxy, direction);
		if (nextProxy == nullptr) {
			throw Controller_Helper_RuleMover_MoveFailedException("need next proxy", __FILE__, __LINE__);
		}

		auto afterNextProxy = this->getNextProxy(proxiesWithVisibleEntries, nextProxy, direction);
		auto previousProxy = this->getNextProxy(proxiesWithVisibleEntries, proxy, this->flipDirection(direction));

		// step 1: analyze situation
		bool ownProxyHasMultipleVisibleRules = this->countVisibleRulesOnToplevel(proxy) > 1;
		bool nextProxyHasMultipleVisibleRules = this->countVisibleRulesOnToplevel(nextProxy) > 1;
		bool afterNextProxyIsForOwnScript = afterNextProxy != nullptr && afterNextProxy->dataSource == proxy->dataSource;
		bool previousProxyIsForNextRule = previousProxy != nullptr && previousProxy->dataSource == nextProxy->dataSource;

		// normalize - setting cases to false that are not different to handle (to keep situationToTask map small)
		if (nextProxyHasMultipleVisibleRules) {
			afterNextProxyIsForOwnScript = false;
		}
		if (ownProxyHasMultipleVisibleRules) {
			previousProxyIsForNextRule = false;
		}

		std::bitset<4> situation;
		situation[0] = ownProxyHasMultipleVisibleRules;
		situation[1] = nextProxyHasMultipleVisibleRules;
		situation[2] = afterNextProxyIsForOwnScript;
		situation[3] = previousProxyIsForNextRule;

		// compare bitmask values with list above. Left value = last assigned situation!
		std::unordered_map<std::bitset<4>, std::set<Task>> situationToTask = {
			{std::bitset<4>("0000"), {Task::MoveOwnProxy}},
			{std::bitset<4>("0001"), {Task::SplitOwnProxy}},
			{std::bitset<4>("0010"), {Task::SplitForeignProxy}},
			{std::bitset<4>("0011"), {Task::SplitOwnProxy, Task::SplitForeignProxy}},
			{std::bitset<4>("0100"), {Task::MoveOwnEntry, Task::DeleteOwnProxy}},
			{std::bitset<4>("0101"), {Task::MoveOwnEntry}},
			{std::bitset<4>("1000"), {Task::MoveForeignEntry, Task::DeleteForeignProxy}},
			{std::bitset<4>("1010"), {Task::MoveForeignEntry}},
			{std::bitset<4>("1100"), {
				Task::MoveOwnEntry, Task::MoveForeignEntry, Task::DeleteOwnProxy, Task::DeleteForeignProxy
			}}
		};

		// step 2: execute tasks
		if (situationToTask.find(situation) == situationToTask.end()) {
			throw LogicException("current situation was not defined. Programming error!", __FILE__, __LINE__);
		}
		auto currentTaskList = situationToTask[situation];

		// it's important to handle all tasks!
		if (currentTaskList.count(Task::MoveOwnProxy)) {
			this->log("Task::MoveOwnProxy", Logger::DEBUG);
			this->moveProxy(proxy, nextProxy, direction);
		}

		if (currentTaskList.count(Task::MoveOwnEntry)) {
			this->log("Task::MoveOwnEntry", Logger::DEBUG);
			this->moveRuleToOtherProxy(rule, proxy, afterNextProxy, direction);
		}

		if (currentTaskList.count(Task::MoveForeignEntry)) {
			this->log("Task::MoveForeignEntry", Logger::DEBUG);
		}

		if (currentTaskList.count(Task::SplitOwnProxy)) {
			this->log("Task::SplitOwnProxy", Logger::DEBUG);
		}

		if (currentTaskList.count(Task::SplitForeignProxy)) {
			this->log("Task::SplitForeignProxy", Logger::DEBUG);
		}

		if (currentTaskList.count(Task::DeleteOwnProxy)) {
			this->log("Task::DeleteOwnProxy", Logger::DEBUG);
		}

		if (currentTaskList.count(Task::DeleteForeignProxy)) {
			this->log("Task::DeleteForeignProxy", Logger::DEBUG);
		}
	}

	private: void moveProxy(
		std::shared_ptr<Model_Proxy> proxyToMove,
		std::shared_ptr<Model_Proxy> destination, // proxyToMove will be moved behind destination
		Controller_Helper_RuleMover_AbstractStrategy::Direction direction
	) {
		auto insertPosition = std::find(this->grublistCfg->proxies.begin(), this->grublistCfg->proxies.end(), destination);
		if (direction == Controller_Helper_RuleMover_AbstractStrategy::Direction::DOWN) {
			insertPosition++;
		}
		auto elementPosition = std::find(this->grublistCfg->proxies.begin(), this->grublistCfg->proxies.end(), proxyToMove);

		this->grublistCfg->proxies.splice(insertPosition, this->grublistCfg->proxies, elementPosition);
	}

	private: void moveRuleToOtherProxy(
		std::shared_ptr<Model_Rule> ruleToMove,
		std::shared_ptr<Model_Proxy> sourceProxy,
		std::shared_ptr<Model_Proxy> destination,
		Controller_Helper_RuleMover_AbstractStrategy::Direction direction
	) {
		// replace ruleToMove by an invisible copy
		auto ruleToMoveSource = std::find(sourceProxy->rules.begin(), sourceProxy->rules.end(), ruleToMove);
		auto dummyRule = std::make_shared<Model_Rule>(*ruleToMove);
		dummyRule->setVisibility(false);
		*ruleToMoveSource = dummyRule;

		// remove old equivalent rule
		destination->removeEquivalentRules(ruleToMove);

		// do the insertion
		auto insertPosition = destination->rules.begin();
		if (direction == Controller_Helper_RuleMover_AbstractStrategy::Direction::UP) {
			insertPosition = destination->rules.end();
		}

		destination->rules.insert(insertPosition, ruleToMove);
	}

	private: std::list<std::shared_ptr<Model_Proxy>> findProxiesWithVisibleToplevelEntries()
	{
		std::list<std::shared_ptr<Model_Proxy>> proxies;

		for (auto proxy : this->grublistCfg->proxies) {
			for (auto rule : proxy->rules) {
				if (rule->isVisible) {
					proxies.push_back(proxy);
					proxy->hasVisibleRules();
					break;
				}
			}
		}

		return proxies;
	}

	private: std::shared_ptr<Model_Proxy> getNextProxy(
		std::list<std::shared_ptr<Model_Proxy>> list,
		std::shared_ptr<Model_Proxy> base,
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

	private: unsigned int countVisibleRulesOnToplevel(std::shared_ptr<Model_Proxy> proxy)
	{
		unsigned int count = 0;

		for (auto rule : proxy->rules) {
			if (rule->isVisible) {
				count++;
			}
		}

		return count;
	}

	private: Controller_Helper_RuleMover_AbstractStrategy::Direction flipDirection(
		Controller_Helper_RuleMover_AbstractStrategy::Direction in
	) {
		if (in == Controller_Helper_RuleMover_AbstractStrategy::Direction::UP) {
			return Controller_Helper_RuleMover_AbstractStrategy::Direction::DOWN;
		}

		if (in == Controller_Helper_RuleMover_AbstractStrategy::Direction::DOWN) {
			return Controller_Helper_RuleMover_AbstractStrategy::Direction::UP;
		}

		throw LogicException("cannot handle given direction", __FILE__, __LINE__);
	}
};
#endif
