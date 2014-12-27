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

#ifndef SRC_MODEL_MAPPER_ABSTRACT_PROCESSREADER_ASYNCWORKER_HPP_
#define SRC_MODEL_MAPPER_ABSTRACT_PROCESSREADER_ASYNCWORKER_HPP_

#include "AbstractWorker.hpp"
#include <iostream>
#include <string>
#include <cstdio>
#include <queue>
#include <memory>
#include "../../../../Controller/Helper/Thread.hpp"
#include "../../../../lib/Mutex.hpp"

class Model_Mapper_Abstract_ProcessReader_ASyncWorker :
	public Model_Mapper_Abstract_ProcessReader_AbstractWorker,
	public std::enable_shared_from_this<Model_Mapper_Abstract_ProcessReader_ASyncWorker>,
	public Controller_Helper_Thread_Connection,
	public Mutex_Connection
{
	private: int dispatcher1_count = 0;
	private: int result = -1;
	private: std::queue<char> receivedChars;
	private: bool running = false;

	public: void run()
	{
		if (this->command == "") {
			throw std::logic_error("a command must be set before running ProcessReader_*::run");
		}

		if (this->running) {
			throw std::logic_error(std::string(__PRETTY_FUNCTION__) + " can only be used once");
		} else {
			this->running = true;
		}
		this->threadHelper->runAsThread(std::bind(std::mem_fn(&Model_Mapper_Abstract_ProcessReader_ASyncWorker::runThread), this->shared_from_this()));
	}

	private: void runThread()
	{
		FILE* process = popen(this->command.c_str(), "r");

		int c;

		while ((c = fgetc(process)) != EOF) {
			if (this->onReceive) {
				auto lock = this->mutex->getLock();
				receivedChars.push(static_cast<char>(c));
				if (this->dispatcher1_count == 0) {
					this->dispatcher1_count++;
					lock->release();
					this->threadHelper->runDispatched(std::bind(std::mem_fn(&Model_Mapper_Abstract_ProcessReader_ASyncWorker::dispatchReceive), this->shared_from_this()));
				}
			}
		}

		this->result = pclose(process);

		if (this->onFinish) {
			this->threadHelper->runDispatched(std::bind(std::mem_fn(&Model_Mapper_Abstract_ProcessReader_ASyncWorker::dispatchFinish), this->shared_from_this()));
		}
	}

	private: void dispatchReceive()
	{
		auto lock = this->mutex->getLock();

		while (this->receivedChars.size()) {
			char c = this->receivedChars.front();
			this->receivedChars.pop();
			lock->release();
			this->onReceive(c);
			lock->acquire();
		}

		dispatcher1_count--;
	}

	private: void dispatchFinish()
	{
		auto lock = this->mutex->getLock();
		int result = this->result;
		lock->release();

		this->onFinish(result);
	}
};


#endif /* SRC_MODEL_MAPPER_ABSTRACT_PROCESSREADER_ASYNCWORKER_HPP_ */
