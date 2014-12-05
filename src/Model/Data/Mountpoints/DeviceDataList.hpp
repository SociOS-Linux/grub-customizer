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

#ifndef SRC_MODEL_DATA_MOUNTPOINTS_DEVICEDATALIST_HPP_
#define SRC_MODEL_DATA_MOUNTPOINTS_DEVICEDATALIST_HPP_

#include <map>
#include <string>

typedef std::map<std::string, std::map<std::string, std::string> > ddl_value_map;

class Model_Data_Mountpoints_DeviceDataList
{
	/**
	 * data structure:
	 * first string key: device name (eg. /dev/sda1)
	 * second string key: property name (eg. UUID)
	 */
	private: ddl_value_map data;

	/**
	 * overwrite complete map
	 */
	public: void setData(ddl_value_map const& data)
	{
		this->data = data;
	}

	public: ddl_value_map& getData()
	{
		return this->data;
	}

	public: ddl_value_map const& getData() const
	{
		return this->data;
	}

	/**
	 * set a property to existing map
	 */
	public: void setProperty(std::string const& deviceName, std::string const& propertyName, std::string const& value)
	{
		this->data[deviceName][propertyName] = value;
	}

	/**
	 * gives direct access to a property
	 */
	public: std::string& getProperty(std::string const& deviceName, std::string const& propertyName)
	{
		return this->data[deviceName][propertyName];
	}

	/**
	 * remove all data
	 */
	public: void clear()
	{
		this->data.clear();
	}

	/**
	 * return the size of the array
	 */
	public: int size()
	{
		return this->data.size();
	}
};

class Model_Data_Mountpoints_DeviceDataList_Connection {
	protected: Model_Data_Mountpoints_DeviceDataList* deviceDataList;
	public:	Model_Data_Mountpoints_DeviceDataList_Connection() : deviceDataList(NULL) {}

	public:	void setDeviceDataList(Model_Data_Mountpoints_DeviceDataList& deviceDataList)
	{
		this->deviceDataList = &deviceDataList;
	}
};

#endif /* SRC_MODEL_DATA_MOUNTPOINTS_DEVICEDATALIST_HPP_ */
