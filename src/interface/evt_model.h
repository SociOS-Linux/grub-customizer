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

#ifndef EVENTLISTENER_MODEL_INCLUDED
#define EVENTLISTENER_MODEL_INCLUDED

#include <string>

/**
 * base class to be implemented by an event listener which is responsible for all model events
 */
class EventListener_model {
public:
	//some new data is loaded
	virtual void loadProgressChanged()=0;
	//save progress continued
	virtual void saveProgressChanged()=0;
	//grub install completely installed
	virtual void grubInstallCompleted(std::string const& msg)=0;
	//framebuffer resolutions has been loaded
	virtual void fb_resolutions_loaded()=0;
};

#endif
