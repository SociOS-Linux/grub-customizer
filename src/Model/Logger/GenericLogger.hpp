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
#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <string>
#include <memory>

namespace Gc { namespace Model { namespace Logger { class GenericLogger
{
	public: virtual inline ~GenericLogger() {};

	public: static std::shared_ptr<Gc::Model::Logger::GenericLogger>& getInstance()
	{
		static std::shared_ptr<Gc::Model::Logger::GenericLogger> logger;

		return logger;
	}

	public: enum Priority {
		EVENT,
		IMPORTANT_EVENT,
		INFO,
		WARNING,
		ERROR,
		DEBUG,
		EXCEPTION
	};

	public: virtual void log(std::string const& str, Priority prio) = 0;
	public: virtual void logActionBegin(std::string const& controller, std::string const& action) = 0;
	public: virtual void logActionEnd() = 0;
	public: virtual void logActionBeginThreaded(std::string const& controller, std::string const& action) = 0;
	public: virtual void logActionEndThreaded() = 0;
};}}}

#endif
