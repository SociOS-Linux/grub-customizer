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

#ifndef LOGGERCONNECTOR_H_
#define LOGGERCONNECTOR_H_
#include "../Logger.h"
#include "LoggerAware.h"
#include "../AutoPtr.h"

class Logger_Connector : public BootstrapInterface_Connector {
	AutoPtr<Logger> logger;
public:
	Logger_Connector(AutoPtr<Logger> logger) : logger(logger) {}

	void connect(BootstrapInterface_Bootstrappable& target) {
		dynamic_cast<Trait_LoggerAware&>(target).setLogger(this->logger);
	}
};

#endif /* LOGGERCONNECTOR_H_ */
