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

#include "../Controller/GLib/ThreadController.hpp"
#include "../lib/Mutex/GLib.hpp"
#include "Thread.hpp"

Bootstrap_Thread::Bootstrap_Thread()
	: mutex1(NULL)
	, mutex2(NULL)
	, threadController(NULL)
{
	Glib::thread_init();

	this->mutex1 = new Mutex_GLib;
	this->mutex2 = new Mutex_GLib;
	this->threadController = new GLib_ThreadController;
}

Bootstrap_Thread::~Bootstrap_Thread() {
	delete this->mutex1;
	delete this->mutex2;
	delete this->threadController;
}
