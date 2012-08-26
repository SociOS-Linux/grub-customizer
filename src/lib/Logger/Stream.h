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

#ifndef STREAM_LOGGER_H_
#define STREAM_LOGGER_H_
#include "../Logger.h"
#include <ostream>

class Logger_Stream : public Logger {
	std::ostream* stream;
public:
	enum LogLevel {
		LOG_NOTHING,
		LOG_DEBUG_ONLY,
		LOG_IMPORTANT,
		LOG_EVENT,
		LOG_VERBOSE
	} logLevel;
	Logger_Stream(std::ostream& stream);
	void log(std::string const& message, Logger::Priority prio);
	void setLogLevel(LogLevel level);
};

#endif
