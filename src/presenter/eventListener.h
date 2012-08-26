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

#ifndef EVENTLISTENER_INCLUDED
#define EVENTLISTENER_INCLUDED

#include "../interface/evt_model.h"
#include "../Controller/ControllerCollection.h"

class EventListener :
	public EventListener_model
{
	ControllerCollection& _controllers;
public:
	EventListener(ControllerCollection& controllers);
	void rootSelectorCompleted();
	void installGrub_request(std::string const& device);

	//model
	void loadProgressChanged();
	void saveProgressChanged();
	void grubInstallCompleted(std::string const& msg);
	void fb_resolutions_loaded();
};
#endif
