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

#ifndef ERRORCONTROLLERIMPL_H_
#define ERRORCONTROLLERIMPL_H_

#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.h"

#include "../Model/Env.h"

#include "../View/Error.h"

#include "../Controller/ControllerAbstract.h"

#include "ErrorController.h"

class ErrorControllerImpl : public ControllerAbstract, public ErrorController {
	Model_Env& env;
	View_Error* view;
	ThreadController* threadController;
	bool applicationStarted;
public:
	void setView(View_Error& view);
	void setThreadController(ThreadController& threadController);
	void setApplicationStarted(bool val);

	ErrorControllerImpl(Model_Env& env);
	
	void errorAction(Exception const& e);
	void errorThreadedAction(Exception const& e);
	void quitAction();
};

#endif
