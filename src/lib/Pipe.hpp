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

#include <sys/types.h>
#include <unistd.h>

class Pipe
{
	private: int readDescriptor;
	private: int writeDescriptor;

	public: class Iterator
	{
		private: Pipe* pipe;

		private: char current;
		private: bool eof;

		private: std::string buffer;
		private: unsigned int bufferPos;

		public: Iterator(Pipe& pipe, bool eof = false)
			: pipe(&pipe), current('\0'), eof(eof), bufferPos(-1)
		{
			if (!eof) {
				(*this)++;
			}
		}

		public: char operator++(int)
		{
			char old = this->current;
			try {
				this->current = this->readNext();
			} catch (std::runtime_error const& e) {
				this->eof = true;
			}
			return old;
		}

		public: char operator++()
		{
			(*this)++;
			return this->current;
		}

		public: char operator*()
		{
			if (this->eof) {
				throw std::runtime_error("reached end of file!");
			}
			return this->current;
		}

		public: bool operator==(Iterator const& other)
		{
			return this->pipe == other.pipe && this->eof == other.eof;
		}

		public: bool operator!=(Iterator const& other)
		{
			return !(*this == other);
		}

		private: char readNext()
		{
			this->bufferPos++;
			if (this->bufferPos < this->buffer.size()) {
				return buffer[bufferPos];
			}
			this->buffer = this->pipe->read(1024);
			this->bufferPos = 0;

			if (this->buffer.size() == 0) {
				throw std::runtime_error("reached end of file!");
			}

			return this->buffer[0];
		}
	};

	public: Pipe()
	{
		int pipeDescriptors[2];
		if (::pipe(pipeDescriptors) == -1) {
			throw std::runtime_error("pipe creation failed");
		}

		this->readDescriptor  = pipeDescriptors[0];
		this->writeDescriptor = pipeDescriptors[1];
	}

	public: char read()
	{
		char buf[1];
		ssize_t length = ::read(this->readDescriptor, buf, 1);
		switch (length) {
			case 0:
				throw std::runtime_error("end of file reached!");
				break;
			case -1:
				throw std::runtime_error("error reading from pipe");
				break;
		}
		return buf[0];
	}

	public: std::string read(int maxSize)
	{
		std::vector<char> buffer(maxSize + 1);
		ssize_t length = ::read(this->readDescriptor, buffer.data(), maxSize);
		if (length == -1) {
			throw std::runtime_error("error reading from pipe");
		}
		return std::string(buffer.data(), length);
	}

	public: Iterator begin()
	{
		return Iterator(*this);
	}

	public: Iterator end()
	{
		return Iterator(*this, true);
	}

	public: void write(char c)
	{
		::write(this->writeDescriptor, &c, 1);
	}

	public: void write(std::string const& str)
	{
		::write(this->writeDescriptor, str.c_str(), str.size());
	}

	public: void closeReadDescriptor()
	{
		::close(this->readDescriptor);
	}

	public: void closeWriteDescriptor()
	{
		::close(this->writeDescriptor);
	}

	public: void mapReadDescriptor(int to)
	{
		::dup2(this->readDescriptor, to);
		this->closeReadDescriptor();
	}

	public: void mapWriteDescriptor(int to)
	{
		::dup2(this->writeDescriptor, to);
		this->closeWriteDescriptor();
	}
};

#endif /* SRC_LIB_PIPE_HPP_ */
