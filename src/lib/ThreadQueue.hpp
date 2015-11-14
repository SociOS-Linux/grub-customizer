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

#ifndef THREADQUEUE_HPP_
#define THREADQUEUE_HPP_

#include <mutex>
#include <queue>
#include <functional>
#include <iostream>
#include <condition_variable>

class ThreadQueue
{
	private: std::mutex mutex;
	private: std::condition_variable condition_push;
	private: std::condition_variable condition_pop;

	private: std::queue<char> objects;
	private: unsigned int maxSize;

	public: std::function<void()> onReceive;

	public: ThreadQueue(unsigned int maxSize) :
		maxSize(maxSize)
	{}

	public: void push(char const& object)
	{
		std::unique_lock<std::mutex> lock(this->mutex);

		this->condition_push.wait(lock, [&] {return this->objects.size() < this->maxSize;});

		this->condition_pop.notify_one();

		this->objects.push(object);

		// we have to unlock the mutex to allow callback function to receive data
		lock.unlock();// after this line, only call thread safe functions of the current object
		auto onReceive = this->onReceive; // copy function pointer to prevent race conditions
		if (onReceive != nullptr) {
			onReceive();
		}
	}

	public: char pop()
	{
		std::unique_lock<std::mutex> lock(this->mutex);

		this->condition_pop.wait(lock, [&] {return this->objects.size() > 0;});

		char value = this->objects.front();
		this->objects.pop();

		this->condition_push.notify_one();

		return value;
	}
};

#endif /* THREADQUEUE_HPP_ */
