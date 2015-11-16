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

#ifndef ASYNCREADERTEST_HPP_
#define ASYNCREADERTEST_HPP_

#include <iostream>

#include "../lib/Parser/Char.hpp"
#include "../lib/AsyncReader.hpp"
#include "../lib/Process.hpp"
#include "../lib/Dispatcher/Glib.hpp"

#include <gtkmm.h>

int main(int argc, char** argv)
{
	Gtk::Main app(argc, argv);

	Gtk::Window win;

	win.set_default_size(400, 300);
	Gtk::TextView tv;
	win.add(tv);
	win.show_all();

	auto pipe = std::make_shared<Pipe>();

	Process::create("grub-mkconfig")
		->setStdErr(pipe->getWriter())
		->setStdOut("/dev/null")
		->run();

	AsyncReader<char>::create()
		->setDispatcher(std::make_shared<Dispatcher_Glib>())
		->setParser(std::make_shared<Parser_Char>(pipe->getReader()))
		->onReceive(
			[&tv] (char c) {
				tv.get_buffer()->insert_at_cursor(Glib::ustring("") + c);
			}
		)
		->onFinish(
			[&tv] {
				tv.get_buffer()->insert_at_cursor(Glib::ustring("\n[END]"));
			}
		)
		->run();

	Gtk::Main::run(win);
}

#endif /* ASYNCREADERTEST_HPP_ */
