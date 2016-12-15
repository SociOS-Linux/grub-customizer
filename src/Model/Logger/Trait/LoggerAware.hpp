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


#include <memory>

namespace Gc { namespace Model { namespace Logger { namespace Trait { class LoggerAware
{
	protected: mutable std::shared_ptr<Gc::Model::Logger::GenericLogger> logger;

	public: virtual ~LoggerAware() {}

	public: LoggerAware()
	{
		if (Gc::Model::Logger::GenericLogger::getInstance()) {
			this->logger = Gc::Model::Logger::GenericLogger::getInstance();
		}
	}

	public: void setLogger(std::shared_ptr<Gc::Model::Logger::GenericLogger> logger) {
		this->logger = logger;
		this->initLogger();
	}

	public: virtual void initLogger() {
		// override to add initializations
	}

	public: std::shared_ptr<Gc::Model::Logger::GenericLogger> getLogger() {
		return this->logger;
	}

	public: bool hasLogger() const
	{
		return this->logger != nullptr;
	}

	protected: void log(std::string const& message, Gc::Model::Logger::GenericLogger::Priority prio) const
	{
		if (this->logger) {
			this->logger->log(message, prio);
		}
	}
};}}}}


