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

#ifndef SRC_LIB_PIPE_HPP_
#define SRC_LIB_PIPE_HPP_

#include <string>
#include <vector>
#include <memory>

#include <sys/types.h>
#include <unistd.h>

#include "Stream.hpp"

class Pipe
{
	private: std::shared_ptr<InputStream> readEnd;
	private: std::shared_ptr<OutputStream> writeEnd;


	public: Pipe()
	{
		int pipeDescriptors[2];
		if (::pipe(pipeDescriptors) == -1) {
			throw std::runtime_error("pipe creation failed");
		}

		this->readEnd  = std::make_shared<InputStream>(pipeDescriptors[0]);
		this->writeEnd = std::make_shared<OutputStream>(pipeDescriptors[1]);
	}

	/**
	 * comfort function to create the pipe as shared_ptr
	 */
	public: static std::shared_ptr<Pipe> create()
	{
		return std::make_shared<Pipe>();
	}

	public: std::shared_ptr<InputStream> getReader()
	{
		return this->readEnd;
	}

	public: std::shared_ptr<OutputStream> getWriter()
	{
		return this->writeEnd;
	}
};

#endif /* SRC_LIB_PIPE_HPP_ */
