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

#ifndef INC_Gc_Controller_Helper_RuleMover_Strategy_MoveRuleOutOfProxyOnToplevel
#define INC_Gc_Controller_Helper_RuleMover_Strategy_MoveRuleOutOfProxyOnToplevel

#include "../../../../Model/ListCfg/Rule.hpp"
#include "../../../../Model/ListCfg/ListCfg.hpp"
#include "../AbstractStrategy.hpp"
#include "../../../../Model/Logger/Trait/LoggerAware.hpp"
#include <memory>
#include <bitset>
#include <set>
#include <unordered_map>

namespace Gc { namespace Controller { namespace Helper { namespace RuleMover { namespace Strategy {
class MoveRuleOutOfProxyOnToplevel :
	public Gc::Controller::Helper::RuleMover::AbstractStrategy,
	public Gc::Model::ListCfg::ListCfgConnection,
	public Gc::Model::Logger::Trait::LoggerAware
{
	private: enum Task
	{
		MoveOwnProxy,
		MoveOwnEntry,
		MoveForeignEntry,
		SplitOwnProxy,
		SplitForeignProxy,
		DeleteOwnProxy,
		DeleteForeignProxy,
		MoveNewProxiesToTheMiddle
	};

	public: MoveRuleOutOfProxyOnToplevel()
		: Gc::Controller::Helper::RuleMover::AbstractStrategy("MoveRuleOutOfProxyOnToplevel")
	{}

	public: void move(std::shared_ptr<Gc::Model::ListCfg::Rule> rule, Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction)
	{
		auto proxy = this->grublistCfg->proxies.getProxyByRule(rule);
		auto proxiesWithVisibleEntries = this->findProxiesWithVisibleToplevelEntries(this->grublistCfg->proxies);

		auto nextProxy = this->getNextProxy(proxiesWithVisibleEntries, proxy, direction);
		if (nextProxy == nullptr) {
			throw Gc::Controller::Helper::RuleMover::MoveFailedException("need next proxy", __FILE__, __LINE__);
		}

		auto afterNextProxy = this->getNextProxy(proxiesWithVisibleEntries, nextProxy, direction);
		auto previousProxy = this->getNextProxy(proxiesWithVisibleEntries, proxy, this->flipDirection(direction));

		auto firstVisibleRuleOfNextProxy = this->getFirstVisibleRule(nextProxy, direction);

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
			{std::bitset<4>("0011"), {Task::SplitForeignProxy, Task::SplitOwnProxy, Task::MoveNewProxiesToTheMiddle}},
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
			throw Gc::Common::LogicException("cannot handle current situation. Programming error!", __FILE__, __LINE__);
		}
		auto currentTaskList = situationToTask[situation];

		// it's important to handle all tasks!
		if (currentTaskList.count(Task::MoveOwnProxy)) {
			this->log("using Task::MoveOwnProxy", Gc::Model::Logger::GenericLogger::INFO);
			this->moveProxy(proxy, nextProxy, direction);
		}

		if (currentTaskList.count(Task::MoveOwnEntry)) {
			this->log("using Task::MoveOwnEntry", Gc::Model::Logger::GenericLogger::INFO);
			this->moveRuleToOtherProxy(rule, proxy, afterNextProxy, direction);
		}

		if (currentTaskList.count(Task::MoveForeignEntry)) {
			this->log("using Task::MoveForeignEntry", Gc::Model::Logger::GenericLogger::INFO);
			this->moveRuleToOtherProxy(firstVisibleRuleOfNextProxy, nextProxy, previousProxy, this->flipDirection(direction));
		}

		if (currentTaskList.count(Task::SplitOwnProxy)) {
			this->log("using Task::SplitOwnProxy", Gc::Model::Logger::GenericLogger::INFO);
			this->insertAsNewProxy(rule, proxy, nextProxy, this->grublistCfg, direction);
		}

		if (currentTaskList.count(Task::SplitForeignProxy)) {
			this->log("using Task::SplitForeignProxy", Gc::Model::Logger::GenericLogger::INFO);
			this->insertAsNewProxy(firstVisibleRuleOfNextProxy, nextProxy, proxy, this->grublistCfg, this->flipDirection(direction));
		}

		if (currentTaskList.count(Task::MoveNewProxiesToTheMiddle)) {
			this->log("using Task::MoveNewProxiesToTheMiddle", Gc::Model::Logger::GenericLogger::INFO);
			this->moveNewProxiesToTheMiddle(proxy, nextProxy, direction);
		}

		if (currentTaskList.count(Task::DeleteOwnProxy)) {
			this->log("using Task::DeleteOwnProxy", Gc::Model::Logger::GenericLogger::INFO);
			this->grublistCfg->proxies.deleteProxy(proxy);
		}

		if (currentTaskList.count(Task::DeleteForeignProxy)) {
			this->log("using Task::DeleteForeignProxy", Gc::Model::Logger::GenericLogger::INFO);
			this->grublistCfg->proxies.deleteProxy(nextProxy);
		}
	}

	private: void moveProxy(
		std::shared_ptr<Gc::Model::ListCfg::Proxy> proxyToMove,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> destination, // proxyToMove will be moved behind destination
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		auto insertPosition = std::find(this->grublistCfg->proxies.begin(), this->grublistCfg->proxies.end(), destination);
		if (direction == Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN) {
			insertPosition++;
		}
		auto elementPosition = std::find(this->grublistCfg->proxies.begin(), this->grublistCfg->proxies.end(), proxyToMove);

		this->grublistCfg->proxies.splice(insertPosition, this->grublistCfg->proxies, elementPosition);

		this->grublistCfg->renumerate();
	}

	private: void moveNewProxiesToTheMiddle(
		std::shared_ptr<Gc::Model::ListCfg::Proxy> oldOwnProxy,
		std::shared_ptr<Gc::Model::ListCfg::Proxy> oldNextProxy,
		Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction
	) {
		auto visibleProxies = this->findProxiesWithVisibleToplevelEntries(this->grublistCfg->proxies);
		auto afterNextProxy = this->getNextProxy(visibleProxies, oldNextProxy, direction);
		auto previousProxy = this->getNextProxy(visibleProxies, oldOwnProxy, this->flipDirection(direction));

		this->moveProxy(oldNextProxy, afterNextProxy, direction);
		this->moveProxy(oldOwnProxy, previousProxy, this->flipDirection(direction));
	}
};}}}}}
#endif
