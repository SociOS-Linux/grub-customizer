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

#include "EntryNameImpl.h"

void Mapper_EntryNameImpl::setView(View_Main& view) {
	this->view = &view;
}

std::string Mapper_EntryNameImpl::map(Model_Entry const* sourceEntry, std::string const& defaultName, std::string const& scriptName) {
	assert(this->view != NULL);
	std::string name;
	bool is_other_entries_ph = sourceEntry ? sourceEntry->type == Model_Entry::SUBMENU || sourceEntry->type == Model_Entry::SCRIPT_ROOT : false;
	bool is_plaintext = sourceEntry ? sourceEntry->type == Model_Entry::PLAINTEXT : false;
	if (is_other_entries_ph) {
		if (sourceEntry->type != Model_Entry::SCRIPT_ROOT) {
			name = this->view->createNewEntriesPlaceholderString(sourceEntry->name, scriptName);
		} else {
			name = this->view->createNewEntriesPlaceholderString("", scriptName);
		}
	} else if (is_plaintext) {
		name = this->view->createPlaintextString(scriptName);
	} else {
		name = defaultName;
	}
	return name;
}