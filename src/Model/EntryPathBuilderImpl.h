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


#ifndef ENTRYPATHBUILDERIMPL_H_
#define ENTRYPATHBUILDERIMPL_H_
#include "../interface/entryPathBuilder.h"
#include "Script.h"
#include <map>

class EntryPathBuilderImpl : public EntryPathBilder {
	Script const* mainScript;
	std::map<Entry const*, Script const*> entrySourceMap;
	std::map<Script const*, std::string> scriptTargetMap;
public:
	EntryPathBuilderImpl(Script const& mainScript);
	void setMainScript(Script const& mainScript);
	void setEntrySourceMap(std::map<Entry const*, Script const*> const& entrySourceMap);
	void setScriptTargetMap(std::map<Script const*, std::string> const& scriptTargetMap);
	std::list<std::string> buildPath(Entry const& entry) const;
	std::string buildPathString(Entry const& entry, bool withOtherEntriesPlaceholder = false) const;
	std::string buildScriptPath(Entry const& entry) const;
};

#endif
