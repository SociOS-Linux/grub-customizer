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

#ifndef SRC_LIB_STREAM_HPP_
#define SRC_LIB_STREAM_HPP_

#include <string>
#include <sys/types.h>
#include <unistd.h>

class Stream
{
	protected: int descriptor;

	private: int usageCount;

	public: Stream(int descriptor)
		: descriptor(descriptor), usageCount(0)
	{}

	public: void close()
	{
		// usageCount can be 0 if usage where not registered. Otherwise 1 = it was registered once so should be deleted.
		if (this->usageCount > 1) {
			this->usageCount--;
			return;
		}

		this->usageCount = -1;
		::close(this->descriptor);
	}

	public: void map(int to)
	{
		::dup2(this->descriptor, to);
	}

	public: void registerUsage()
	{
		this->usageCount++;
	}

	public: bool isClosed()
	{
		return this->usageCount == -1;
	}

	public: int getDescriptor()
	{
		return this->descriptor;
	}

	public: void setDescriptor(int descriptor)
	{
		this->descriptor = descriptor;
	}
};

class InputStream : public Stream
{
	public: InputStream(int descriptor)
		: Stream(descriptor)
	{}

	public: class Iterator
	{
		private: InputStream* readEnd;

		private: char current;
		private: bool eof;

		private: std::string buffer;
		private: unsigned int bufferPos;

		public: Iterator(InputStream& readEnd, bool eof = false)
			: readEnd(&readEnd), current('\0'), eof(eof), bufferPos(-1)
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
			return this->readEnd == other.readEnd && this->eof == other.eof;
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
			this->buffer = this->readEnd->read(1024, false);
			this->bufferPos = 0;

			if (this->buffer.size() == 0) {
				throw std::runtime_error("reached end of file!");
			}

			return this->buffer[0];
		}
	};

	public: Iterator begin()
	{
		return Iterator(*this);
	}

	public: Iterator end()
	{
		return Iterator(*this, true);
	}

	public: std::string read(int maxSize, bool waitUntilFull = true)
	{
		std::vector<char> buffer(maxSize + 1);
		char* writePosition = buffer.data();
		int currentSize = 0;
		do {
			ssize_t length = ::read(this->descriptor, writePosition, maxSize - currentSize);
			if (length == -1) {
				throw std::runtime_error("error reading from pipe");
			} else if (length == 0) {
				break; // end of stream
			}
			currentSize += length;
			writePosition += length;
		} while (waitUntilFull && currentSize < maxSize);
		return std::string(buffer.data(), currentSize);
	}

	public: char read()
	{
		char buf[1];
		ssize_t length = ::read(this->descriptor, buf, 1);
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
};

class OutputStream : public Stream
{
	public: OutputStream(int descriptor)
		: Stream(descriptor)
	{}

	public: void write(char c)
	{
		::write(this->descriptor, &c, 1);
	}

	public: void write(std::string const& str)
	{
		::write(this->descriptor, str.c_str(), str.size());
	}
};



#endif /* SRC_LIB_STREAM_HPP_ */
