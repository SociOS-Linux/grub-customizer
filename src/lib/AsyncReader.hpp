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

#ifndef ANSYNCREADER_HPP_
#define ANSYNCREADER_HPP_

#include "Dispatcher/Abstract.hpp"
#include "Parser/Abstract.hpp"
#include "ThreadQueue.hpp"

#include <thread>

template <typename T_Res> class AsyncReader : public std::enable_shared_from_this<AsyncReader<T_Res>>
{
	private: std::shared_ptr<Dispatcher_Abstract> dispatcher;
	private: std::shared_ptr<Parser_Abstract<T_Res>> parser;
	private: ThreadQueue<T_Res> results;
	private: ThreadQueue<bool> resultExists; // true for each result object, false after reading the last element
	private: std::shared_ptr<std::thread> thread;

	private: std::function<void(T_Res res)> onReceiveCallback;
	private: std::function<void()> onFinishCallback;

	public: AsyncReader(unsigned int maxBufferSize = 100) :
		results(maxBufferSize),
		resultExists(maxBufferSize)
	{}

	public: static std::shared_ptr<AsyncReader<T_Res>> create(unsigned int maxBufferSize = 100)
	{
		return std::make_shared<AsyncReader<T_Res>>(maxBufferSize);
	}

	public: std::shared_ptr<AsyncReader<T_Res>> setDispatcher(std::shared_ptr<Dispatcher_Abstract> dispatcher)
	{
		this->dispatcher = dispatcher;
		return this->shared_from_this();
	}

	public: std::shared_ptr<AsyncReader<T_Res>> setParser(std::shared_ptr<Parser_Abstract<T_Res>> parser)
	{
		this->parser = parser;
		return this->shared_from_this();
	}

	public: std::shared_ptr<AsyncReader<T_Res>> onReceive(std::function<void(T_Res res)> onReceiveCallback)
	{
		this->onReceiveCallback = onReceiveCallback;
		return this->shared_from_this();
	}

	public: std::shared_ptr<AsyncReader<T_Res>> onFinish(std::function<void()> onFinishCallback)
	{
		this->onFinishCallback = onFinishCallback;
		return this->shared_from_this();
	}

	public: std::shared_ptr<AsyncReader<T_Res>> run()
	{
		if (this->dispatcher == nullptr) {
			throw std::logic_error("need dispatcher");
		}
		if (this->parser == nullptr) {
			throw std::logic_error("need parser");
		}
		if (this->onReceiveCallback == nullptr) {
			this->onReceiveCallback = [] (T_Res v) {};
		}
		if (this->onFinishCallback == nullptr) {
			this->onFinishCallback = [] {};
		}

		auto self = this->shared_from_this();

		// giving a shared pointer to make sure the object wont be destructed before finishing
		this->dispatcher->setCallback(
			[self] {
				if (!self->resultExists.pop()) {
					self->onFinishCallback();
					self->dispatcher->setCallback([]{});
					return;
				}
				self->onReceiveCallback(self->results.pop());
			}
		);
		this->thread = std::make_shared<std::thread>(
			[self] {
				for (auto obj : *self->parser) {
					self->resultExists.push(true);
					self->results.push(obj);
					self->dispatcher->notify();
				}
				self->resultExists.push(false);
				self->dispatcher->notify();
			}
		);
		return self;
	}

	public: ~AsyncReader()
	{
		if (this->thread != nullptr) {
			this->thread->join();
		}
	}
};

#endif /* ANSYNCREADER_HPP_ */
