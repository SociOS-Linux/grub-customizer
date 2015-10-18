/*
 * Copyright (C) 2010-2014 Daniel Richter <danielrichter2007@web.de>
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

#ifndef SRC_MODEL_MAPPER_ABSTRACT_PROCESSREADER_HPP_
#define SRC_MODEL_MAPPER_ABSTRACT_PROCESSREADER_HPP_

#include "ProcessReader/SyncWorker.hpp"
#include "ProcessReader/ASyncWorker.hpp"
#include "../../../Controller/Helper/Thread.hpp"
#include "../../../lib/Mutex.hpp"

class Model_Mapper_Abstract_ProcessReader :
	public Controller_Helper_Thread_Connection,
	public Mutex_Connection
{
	public: void runSync(
		std::string const& command,
		std::function<void (char)> onReceive,
		std::function<void (int)> onFinish
	) const	{
		std::make_shared<Model_Mapper_Abstract_ProcessReader_SyncWorker>(command, onReceive, onFinish);
	}

	public: void runASync(
		std::string const& command,
		std::function<void (char)> onReceive,
		std::function<void (int)> onFinish
	) const	{
		std::make_shared<Model_Mapper_Abstract_ProcessReader_ASyncWorker>(
			command,
			onReceive,
			onFinish,
			this->threadHelper,
			this->mutex
		);
	}
};

#endif /* SRC_MODEL_MAPPER_ABSTRACT_PROCESSREADER_HPP_ */
