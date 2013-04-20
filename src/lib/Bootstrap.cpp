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

#include "Bootstrap.h"

void Bootstrap::push(BootstrapInterface_Bootstrappable& object) {
	this->bootstrappables.push_back(&object);
}

void Bootstrap::pushRessource(BootstrapInterface_Connector& resourceConnector) {
	this->resourceConnectors.push_back(&resourceConnector);
}

void Bootstrap::run() {
	for (std::list<BootstrapInterface_Connector*>::iterator connectorIter = resourceConnectors.begin(); connectorIter != resourceConnectors.end(); connectorIter++) {
		for (std::list<BootstrapInterface_Bootstrappable*>::iterator bootstrappableIter = bootstrappables.begin(); bootstrappableIter != bootstrappables.end(); bootstrappableIter++) {
			try {
				(*connectorIter)->connect(**bootstrappableIter);
			} catch (std::bad_cast const& e) {}
		}
	}
}

Bootstrap& Bootstrap::operator<<(BootstrapInterface_Bootstrappable& object) {
	this->bootstrappables.push_back(&object);
	return *this;
}
