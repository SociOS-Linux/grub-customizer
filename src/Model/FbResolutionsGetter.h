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

#ifndef FB_RESOLUTIONS_GETTER
#define FB_RESOLUTIONS_GETTER
#include <string>
#include <list>
#include <cstdio>
#include "../interface/evt_model.h"
#include "../presenter/commonClass.h"

class Model_FbResolutionsGetter : public CommonClass {
	std::list<std::string> data;
	EventListener_model* eventListener;
	bool _isLoading;
public:
	Model_FbResolutionsGetter();
	const std::list<std::string>& getData() const;
	void load();
	void setEventListener(EventListener_model& eventListener);
};

#endif
