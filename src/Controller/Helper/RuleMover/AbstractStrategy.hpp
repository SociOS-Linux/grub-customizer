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

class Controller_Helper_RuleMover_AbstractStrategy
{
	public: enum class Direction {
		DOWN = 1,
		UP = -1
	};

	public: virtual void move(std::shared_ptr<Model_Rule> rule, Controller_Helper_RuleMover_AbstractStrategy::Direction direction) = 0;
	public: virtual ~Controller_Helper_RuleMover_AbstractStrategy(){};
};

#endif
