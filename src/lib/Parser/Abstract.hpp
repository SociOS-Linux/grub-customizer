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

#ifndef INC_Parser_Abstract
#define INC_Parser_Abstract

template <typename T_Res> class Parser_Abstract
{
	public: virtual T_Res readNext() = 0;

	public: class Iterator
	{
		private: T_Res current;
		private: bool valid;
		private: Parser_Abstract<T_Res>* obj = nullptr;

		public: Iterator(Parser_Abstract<T_Res>& obj, bool valid = false) :
			obj(&obj), valid(valid)
		{
			if (valid) {
				(*this)++;
			}
		}

		public: char operator++(int)
		{
			char old = this->current;
			try {
				this->current = this->obj->readNext();
				this->valid = true;
			} catch (std::runtime_error const& e) {
				this->valid = false;
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
			if (!this->valid) {
				throw std::runtime_error("invalid iterator state");
			}
			return this->current;
		}

		public: bool operator==(Iterator const& other)
		{
			return this->obj == other.obj && this->valid == other.valid;
		}

		public: bool operator!=(Iterator const& other)
		{
			return !(*this == other);
		}
	};

	public: Iterator begin()
	{
		return Iterator(*this, true);
	}

	public: Iterator end()
	{
		return Iterator(*this);
	}

	public: virtual ~Parser_Abstract() {};
};

#endif
