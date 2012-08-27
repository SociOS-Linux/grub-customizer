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
#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <string>

class Logger {
public:
	virtual inline ~Logger() {};

	enum Priority {
		EVENT,
		IMPORTANT_EVENT,
		INFO,
		WARNING,
		ERROR,
		DEBUG,
		EXCEPTION
	};
	virtual void log(std::string const& str, Priority prio) = 0;
};

#endif
