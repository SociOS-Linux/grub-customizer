/*
 * Copyright (C) 2010-2014 Daniel Richter <danielrichter2007@web.de>
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

#ifndef SRC_LIB_TESTPROCESS_HPP_
#define SRC_LIB_TESTPROCESS_HPP_

#include "Process.hpp"

int main()
{
	auto testPipe = std::make_shared<Pipe>();

	Process::create("bash")
		->addArgument("echo all good!; cat /etc/issue3")
		->setStdOut(Process::create("md5sum")->setStdOut(testPipe->getWriter()))
		->setStdErr(Process::create("base64")->setStdOut(testPipe->getWriter()))
		->run();

	std::cout << "finally it's " << (testPipe->getWriter()->isClosed() ? "closed" : "not closed") << std::endl;

//	testPipe->write("Test!");
//	testPipe->closeWriteDescriptor();

	for (char c : *testPipe->getReader()) {
		std::cerr << c;
	}

	std::cout << "pipe reading finished!" << std::endl;
}

#endif /* SRC_LIB_TESTPROCESS_HPP_ */
