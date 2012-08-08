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

#ifndef ENTRY_PATH_FOLLOWER_INCLUDED
#define ENTRY_PATH_FOLLOWER_INCLUDED
#include <string>
#include <list>
#include "../model/entry.h"

class EntryPathFollower {
public:
	virtual inline ~EntryPathFollower() {};

	virtual Entry* getEntryByPath(std::list<std::string> const& path)=0;
};

#endif
