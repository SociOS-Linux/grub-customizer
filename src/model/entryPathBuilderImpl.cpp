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

#include "entryPathBuilderImpl.h"

EntryPathBuilderImpl::EntryPathBuilderImpl(Script const& mainScript) {
	this->setMainScript(mainScript);
}

void EntryPathBuilderImpl::setMainScript(Script const& mainScript) {
	this->mainScript = &mainScript;
}

void EntryPathBuilderImpl::setEntrySourceMap(std::map<Entry const*, Script const*> const& entrySourceMap) {
	this->entrySourceMap = entrySourceMap;
}
void EntryPathBuilderImpl::setScriptTargetMap(std::map<Script const*, std::string> const& scriptTargetMap) {
	this->scriptTargetMap = scriptTargetMap;
}

std::list<std::string> EntryPathBuilderImpl::buildPath(Entry const& entry) const {
	Script const* script = entrySourceMap.find(&entry) != entrySourceMap.end() ? entrySourceMap.find(&entry)->second : this->mainScript;
	return script->buildPath(entry);
}
std::string EntryPathBuilderImpl::buildPathString(Entry const& entry, bool withOtherEntriesPlaceholder) const {
	Script const* script = entrySourceMap.find(&entry) != entrySourceMap.end() ? entrySourceMap.find(&entry)->second : this->mainScript;
	return script->buildPathString(entry, withOtherEntriesPlaceholder);
}

std::string EntryPathBuilderImpl::buildScriptPath(Entry const& entry) const {
	Script const* script = entrySourceMap.find(&entry) != entrySourceMap.end() ? entrySourceMap.find(&entry)->second : NULL;
	return script ? this->scriptTargetMap.find(script)->second : "";
}
