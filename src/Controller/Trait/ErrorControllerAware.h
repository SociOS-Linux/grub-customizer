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

#ifndef ERRORCONTROLLERAWARE_H_
#define ERRORCONTROLLERAWARE_H_
#include <cstdlib>
#include "../ErrorController.h"

class Trait_ErrorControllerAware {
public:
	enum ThreadedType {
		NOT_THREADED,
		THREADED
	};
private:
	ErrorController* errorController;
public:
	Trait_ErrorControllerAware() : errorController(NULL) {}

	void setErrorController(ErrorController& errorController) {
		this->errorController = &errorController;
	}

protected:
	void handleException(Exception const& e, ThreadedType threaded = NOT_THREADED) {
		if (this->errorController) {
			if (threaded == NOT_THREADED) {
				this->errorController->errorAction(e);
			} else {
				this->errorController->errorThreadedAction(e);
			}
		} else {
			throw NullPointerException("cannot handle exception because there's no errorController\n" + e, __FILE__, __LINE__);
		}
	}
};

#endif /* ERRORCONTROLLERAWARE_H_ */
