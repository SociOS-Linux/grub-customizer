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

#ifndef SRC_LIB_FILE_HPP_
#define SRC_LIB_FILE_HPP_

#include <memory>
#include "Stream.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>

class File
{
	public: enum WriteMode {
		REPLACE,
		APPEND
	};

	public: static std::shared_ptr<InputStream> openInputFile(std::string const& path)
	{
		int file = ::open(path.c_str(), 0);

		if (file == -1) {
			throw std::runtime_error("failed opening input file");
		}

		return std::make_shared<InputStream>(file);
	}

	public: static std::shared_ptr<OutputStream> openOutputFile(std::string const& path, WriteMode writeMode = WriteMode::REPLACE)
	{
		int file = ::open(
			path.c_str(),
			O_WRONLY | O_CREAT | (writeMode == WriteMode::REPLACE ? O_TRUNC : O_APPEND)
		);

		if (file == -1) {
			throw std::runtime_error("failed opening output file");
		}

		return std::make_shared<OutputStream>(file);
	}
};

#endif /* SRC_LIB_FILE_HPP_ */
