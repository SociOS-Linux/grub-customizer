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

#ifndef BOOTSTRAP_H_
#define BOOTSTRAP_H_
#include "AutoPtr.h"
#include <list>
#include <typeinfo>

#include "BootstrapInterface/Bootstrappable.h"
#include "BootstrapInterface/Connector.h"

class Bootstrap {
	std::list<AutoPtr<BootstrapInterface_Bootstrappable> > bootstrappables;
	std::list<AutoPtr<BootstrapInterface_Connector> > resourceConnectors;
public:
	AutoPtr<BootstrapInterface_Bootstrappable> push(AutoPtr<BootstrapInterface_Bootstrappable> const& object);
	void pushRessource(AutoPtr<BootstrapInterface_Connector> const& resourceConnector);
	void run();
};


#endif /* BOOTSTRAP_H_ */
