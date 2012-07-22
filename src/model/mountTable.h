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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MOUNT_TABLE_INCLUDED
#define MOUNT_TABLE_INCLUDED
#include <list>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <iostream>


struct Mountpoint {
	enum Exception {
		MOUNT_FAILED,
		UMOUNT_FAILED
	};
	std::string device, mountpoint, fileSystem, options, dump, pass;
	bool isMounted;
	bool isValid(std::string const& prefix = "", bool isRoot = false) const;
	operator bool() const;
	Mountpoint(std::string const& mountpoint = "", bool isMounted = false);
	Mountpoint(std::string const& device, std::string const& mountpoint, std::string const& options, bool isMounted = false);
	void mount();
	void umount();
	bool isLiveCdFs();
};

class MountTable : public std::list<Mountpoint> {
	bool loaded;
	public:
	enum Exception {
		MOUNT_ERR_NO_FSTAB,
		MOUNTPOINT_NOT_FOUND
	};
	MountTable(FILE* source, std::string const& rootDirectory, bool default_isMounted_flag = false);
	MountTable(std::string const& rootDirectory);
	MountTable();
	void sync(MountTable const& mtab);
	bool isLoaded() const;
	operator bool() const;
	void loadData(FILE* source, std::string const& prefix, bool default_isMounted_flag = false);
	void loadData(std::string const& rootDirectory);
	void clear(std::string const& prefix);
	Mountpoint getEntryByMountpoint(std::string const& mountPoint) const;
	Mountpoint& getEntryRefByMountpoint(std::string const& mountPoint);
	Mountpoint& add(Mountpoint const& mpToAdd);
	void remove(Mountpoint const& mountpoint);
	void umountAll(std::string const& prefix);
	void mountRootFs(std::string const& device, std::string const& mountpoint);
	void print() const;
};
#endif

