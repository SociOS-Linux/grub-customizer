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

#ifndef RULEMOVER_HPP_
#define RULEMOVER_HPP_

#include <memory>


namespace Gc { namespace Controller { namespace Helper { class RuleMoverHelper :
	public Gc::Model::ListCfg::ListCfgConnection,
	public Gc::Model::Logger::Trait::LoggerAware
{
	private: std::list<std::shared_ptr<Gc::Controller::Helper::RuleMover::AbstractStrategy>> strategies;

	public: void move(std::shared_ptr<Gc::Model::ListCfg::Rule> rule, Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction direction)
	{
		assert(this->grublistCfg != nullptr);

		for (auto strategy : this->strategies) {
			try {
				this->log("trying move strategy \"" + strategy->getName() + "\"", Gc::Model::Logger::GenericLogger::INFO);
				strategy->move(rule, direction);
				this->log("move strategy \"" + strategy->getName() + "\" was successful", Gc::Model::Logger::GenericLogger::INFO);
				return;
			} catch (Gc::Controller::Helper::RuleMover::MoveFailedException const& e) {
				continue;
			}
		}
		throw Gc::Common::Exception::NoMoveTargetException("cannot move this rule. No successful strategy found", __FILE__, __LINE__);
	}

	public: void addStrategy(std::shared_ptr<Gc::Controller::Helper::RuleMover::AbstractStrategy> strategy)
	{
		this->strategies.push_back(strategy);
	}
};}}}

#endif /* RULEMOVER_HPP_ */
