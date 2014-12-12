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

#ifndef SRC_BOOTSTRAP_APPLICATION_HPP_
#define SRC_BOOTSTRAP_APPLICATION_HPP_
#include <memory>
#include <functional>
#include <list>
#include "../lib/Exception.hpp"

template <typename... Args>
class Bootstrap_Application_Event
{
	private: std::list<std::function<void (Args...)>> eventHandlers;

	public: void addHandler(std::function<void (Args...)> eventHandler)
	{
		this->eventHandlers.push_back(eventHandler);
	}

	public: void exec(Args... args)
	{
		for (auto func : this->eventHandlers) {
			func(args...);
		}
	}
};

class Bootstrap_Application_Object
{
	public: Bootstrap_Application_Event<Exception> onError;
	public: Bootstrap_Application_Event<Exception> onThreadError;

	public: virtual void addShutdownHandler(std::function<void ()> callback) = 0;
	public: virtual void shutdown() = 0;
	public: virtual void run() = 0;
	public: virtual ~Bootstrap_Application_Object(){}
};

class Bootstrap_Application_Object_Connection
{
	protected: std::shared_ptr<Bootstrap_Application_Object> applicationObject;

	public: virtual ~Bootstrap_Application_Object_Connection(){}

	public: void setApplicationObject(std::shared_ptr<Bootstrap_Application_Object> applicationObject)
	{
		this->applicationObject = applicationObject;

		this->initApplicationEvents();
	}

	public: virtual void initApplicationEvents()
	{
		// override to initialize application events
	}
};

class Bootstrap_Application
{
	public: std::shared_ptr<Bootstrap_Application_Object> applicationObject;

	public: Bootstrap_Application(int argc, char** argv);
};

#endif /* SRC_BOOTSTRAP_APPLICATION_HPP_ */
