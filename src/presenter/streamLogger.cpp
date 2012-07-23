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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "streamLogger.h"

StreamLogger::StreamLogger(std::ostream& stream) : stream(&stream) {}

void StreamLogger::log(std::string const& message, Logger::Priority prio) {
	if (this->logLevel == LOG_NOTHING ||
		this->logLevel == LOG_DEBUG_ONLY && prio != Logger::DEBUG ||
		this->logLevel == LOG_IMPORTANT && prio != Logger::IMPORTANT_EVENT ||
		this->logLevel == LOG_EVENT && prio != Logger::EVENT && prio != Logger::IMPORTANT_EVENT) {
		return;
	}
	if (prio == Logger::IMPORTANT_EVENT) {
		*this->stream << " *** ";
	} else if (prio == Logger::EVENT) {
		*this->stream << "   * ";
	} else {
		*this->stream << "     ";
	}

	if (prio == Logger::INFO) {
		*this->stream << "[";
	}
	*this->stream << message;
	if (prio == Logger::INFO) {
		*this->stream << "]";
	}

	*this->stream << std::endl;
}

void StreamLogger::setLogLevel(LogLevel level) {
	this->logLevel = level;
}
