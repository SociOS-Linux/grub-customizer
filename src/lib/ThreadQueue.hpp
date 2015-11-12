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

class ThreadQueue
{
	private: std::mutex accessProtection;
	private: std::mutex overflowProtection;
	private: std::mutex underflowProtection;

	private: bool overflowLocked;
	private: std::queue<char> objects;
	private: unsigned int maxSize;

	public: std::function<void()> onReceive;

	public: ThreadQueue(unsigned int maxSize) :
		maxSize(maxSize), overflowLocked(false)
	{
		// the queue is empty so we have to enable the underflow protection
		this->underflowProtection.lock();
	}

	public: void push(char const& object)
	{
		this->overflowProtection.lock();
		std::lock_guard<std::mutex> lock(this->accessProtection);

		this->objects.push(object);

		this->underflowProtection.unlock();

		if (this->objects.size() < this->maxSize) {
			this->overflowProtection.unlock();
		}

		if (this->onReceive != nullptr) {
			lock.~lock_guard(); // the following function needs access to the object
			this->onReceive();
		}
	}

	public: char pop()
	{
		this->underflowProtection.lock();
		std::lock_guard<std::mutex> lock(this->accessProtection);

		char value = this->objects.front();
		this->objects.pop();

		this->overflowProtection.unlock();

		if (this->objects.size() > 0) {
			this->underflowProtection.unlock();
		}
		return value;
	}
};

#endif /* THREADQUEUE_HPP_ */
