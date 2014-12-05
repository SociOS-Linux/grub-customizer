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

#ifndef SRC_MODEL_DATA_COLLECTION_HPP_
#define SRC_MODEL_DATA_COLLECTION_HPP_

#include "Mountpoints/DeviceDataList.hpp"

class Model_Data_Collection
{
	public: Model_Data_Mountpoints_DeviceDataList deviceDataList;
};

class Model_Data_Collection_Connection
{
	protected: Model_Data_Collection* models;

	public:	Model_Data_Collection_Connection() : models(NULL) {}

	public:	void setModelDataCollection(Model_Data_Collection& collection)
	{
		this->models = &collection;
	}
};

#endif /* SRC_MODEL_DATA_COLLECTION_HPP_ */
