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

#ifndef AUTOPTR_H_
#define AUTOPTR_H_
#include <cstdlib>
#include "Exception.h"

template <typename T>
class AutoPtr {
	T* object;
	int* refCount;
public:
	AutoPtr() : object(NULL), refCount(NULL) {
		refCount = new int(0);
	}

	AutoPtr(T* object) : object(object), refCount(NULL) {
		refCount = new int(1);
	}

	AutoPtr(AutoPtr const& other) : object(NULL), refCount(NULL) {
		this->increase(other);
	}

	~AutoPtr() {
		this->decrease();
	}

	AutoPtr& operator=(AutoPtr& other) {
		this->decrease();

		this->increase(other);
		return other;
	}

	AutoPtr const& operator=(AutoPtr const& other) {
		this->decrease();

		this->increase(other);
		return other;
	}

	T& operator*() {
		if (object == NULL) {
			throw NullPointerException("nullpointer dereference in AutoPtr", __FILE__, __LINE__);
		}
		return *object;
	}

	T* operator->() {
		if (object == NULL) {
			throw NullPointerException("nullpointer dereference in AutoPtr", __FILE__, __LINE__);
		}
		return object;
	}

	operator bool() {
		return *refCount != 0;
	}

private:
	void increase(AutoPtr const& other) {
		this->object = other.object;
		this->refCount = other.refCount;
		(*refCount)++;
	}

	void decrease() {
		if (refCount == NULL || object == NULL) {
			return;
		}
		(*refCount)--;
		if (*refCount == 0) {
			delete refCount;
			delete object;

			refCount = NULL;
			object = NULL;
		}
	}
};

#endif /* AUTOPTR_H_ */
