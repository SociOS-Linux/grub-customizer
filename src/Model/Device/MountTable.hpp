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

#ifndef MOUNT_TABLE_INCLUDED
#define MOUNT_TABLE_INCLUDED
#include <list>
#include <cstdio>
#include <string>
#include <cstdlib>

#include "../../Common/Exception.hpp"
#include "../Logger/Trait/LoggerAware.hpp"
#include "../../Common/Functions.hpp"
#include "MountTableMountpoint.hpp"


namespace Gc { namespace Model { namespace Device { class MountTable :
	public std::list<Gc::Model::Device::MountTableMountpoint>,
	public Gc::Model::Logger::Trait::LoggerAware
{
	private: bool loaded;

	public: MountTable(FILE* source, std::string const& prefix, bool default_isMounted_flag)
		: loaded(false)
	{
		this->loadData(source, prefix, default_isMounted_flag);
	}

	public: MountTable(std::string const& rootDirectory) : loaded(false)
	{
		this->loadData(rootDirectory);
	}

	public: MountTable() :
		loaded(false)
	{}

	public: void sync(Gc::Model::Device::MountTable const& mtab)
	{
		for (Gc::Model::Device::MountTable::const_iterator iter = mtab.begin(); iter != mtab.end(); iter++){
			this->add(*iter);
		}
	}

	public: bool isLoaded() const
	{
		return loaded;
	}

	public: operator bool() const
	{
		return isLoaded();
	}

	public: void loadData(FILE* source, std::string const& prefix, bool default_isMounted_flag = false)
	{
		int c;
		int rowEntryPos = 0;
		bool isComment = false;
		bool isBeginOfRow = true;
		char previousChar = 0;
		Gc::Model::Device::MountTableMountpoint newMp(prefix, default_isMounted_flag);
		while ((c = fgetc(source)) != EOF){
			if (isBeginOfRow && c == '#') {
				isComment = true;
			} else if (c == '\n'){
				this->add(prefix, newMp);

				newMp = Gc::Model::Device::MountTableMountpoint(prefix, default_isMounted_flag);
				rowEntryPos = 0;
				isBeginOfRow = true;
				isComment = false;
			} else if (!isComment) {
				if (c == ' ' || c == '\t'){
					if (previousChar != ' ' && previousChar != '\t')
						rowEntryPos++;
					isBeginOfRow = false;
				} else {
					switch (rowEntryPos){
						case 0:	newMp.device += char(c); break;
						case 1: newMp.mountpoint += char(c); break;
						case 2: newMp.fileSystem += char(c); break;
						case 3: newMp.options += char(c); break;
						case 4: newMp.dump += char(c); break;
						case 5: newMp.pass += char(c); break;
					}
					isBeginOfRow = false;
				}
			}
			previousChar = c;
		}
		this->add(prefix, newMp);

		loaded = true;
	}

	private: void add(std::string const& prefix, Gc::Model::Device::MountTableMountpoint& newMp)
	{
		// decode
		newMp.device     = Gc::Common::Functions::str_replace("\\040", " ", newMp.device);
		newMp.mountpoint = Gc::Common::Functions::str_replace("\\040", " ", newMp.mountpoint);
		newMp.fileSystem = Gc::Common::Functions::str_replace("\\040", " ", newMp.fileSystem);
		newMp.options    = Gc::Common::Functions::str_replace("\\040", " ", newMp.options);
		newMp.dump       = Gc::Common::Functions::str_replace("\\040", " ", newMp.dump);
		newMp.pass       = Gc::Common::Functions::str_replace("\\040", " ", newMp.pass);

		bool isRoot = newMp.mountpoint == prefix + "/";

		if (newMp.mountpoint[newMp.mountpoint.length()-1] == '/') {
			newMp.mountpoint = newMp.mountpoint.substr(0, newMp.mountpoint.length()-1);
		}

		if (newMp.isValid(prefix, isRoot)){
			this->remove(newMp);
			this->push_back(newMp);
		}
	}

	public: void loadData(std::string const& rootDirectory)
	{
		FILE* fstabFile = fopen((rootDirectory+"/etc/fstab").c_str(), "r");
		if (fstabFile != NULL){
			this->loadData(fstabFile, rootDirectory);
			Gc::Model::Device::MountTable mtab;
			FILE* mtabfile = fopen("/etc/mtab", "r"); //use global mtab - the local one is unmanaged
			if (mtabfile){
				mtab = Gc::Model::Device::MountTable(mtabfile, "", true);
				fclose(mtabfile);
			}
			this->sync(mtab);
			fclose(fstabFile);
		}
	}

	public: void clear(std::string const& prefix)
	{
		Gc::Model::Device::MountTable::iterator iter = this->begin();
		while (iter != this->end()){
			if (iter->mountpoint.substr(0, prefix.length()) == prefix){
				this->erase(iter);
				iter = this->begin();
			}
			else
				iter++;
		}
		loaded = false;
	}

	public: Gc::Model::Device::MountTableMountpoint getEntryByMountpoint(std::string const& mountPoint) const
	{
		for (std::list<Gc::Model::Device::MountTableMountpoint>::const_iterator iter = this->begin(); iter != this->end(); iter++){
			if (iter->mountpoint == mountPoint)
				return *iter;
		}
		return Gc::Model::Device::MountTableMountpoint();
	}

	public: Gc::Model::Device::MountTableMountpoint& getEntryRefByMountpoint(std::string const& mountPoint)
	{
		for (std::list<Gc::Model::Device::MountTableMountpoint>::iterator iter = this->begin(); iter != this->end(); iter++){
			if (iter->mountpoint == mountPoint)
				return *iter;
		}
		throw Gc::Common::MountpointNotFoundException("mountpoint not found", __FILE__, __LINE__);
	}

	public: Gc::Model::Device::MountTableMountpoint& add(Gc::Model::Device::MountTableMountpoint const& mpToAdd)
	{
		this->remove(mpToAdd); //remove existing mountpoints with the same directory
		this->push_back(mpToAdd);
		return this->back();
	}

	public: void remove(Gc::Model::Device::MountTableMountpoint const& mountpoint)
	{
		for (std::list<Gc::Model::Device::MountTableMountpoint>::iterator iter = this->begin(); iter != this->end(); iter++){
			if (iter->mountpoint == mountpoint.mountpoint){
				this->erase(iter);
				break;
			}
		}
	}

	public: void umountAll(std::string const& prefix)
	{
		for (Gc::Model::Device::MountTable::reverse_iterator iter = this->rbegin(); iter != this->rend(); iter++){
			if (iter->mountpoint.substr(0, prefix.length()) == prefix && iter->mountpoint != prefix && iter->isMounted){
				iter->umount();
			}
		}
	
		this->getEntryRefByMountpoint(prefix).umount();
	}

	public: void mountRootFs(std::string const& device, std::string const& mountpoint)
	{
		this->add(Gc::Model::Device::MountTableMountpoint(device, mountpoint, "")).mount();
		this->loadData(mountpoint);
		FILE* fstab = fopen((mountpoint + "/etc/fstab").c_str(), "r");
		if (fstab){
			fclose(fstab); //opening of fstab is just a test
	
			try {
				this->add(Gc::Model::Device::MountTableMountpoint("/proc", mountpoint + "/proc", "bind")).mount();
				this->add(Gc::Model::Device::MountTableMountpoint("/sys", mountpoint + "/sys", "bind")).mount();
				this->add(Gc::Model::Device::MountTableMountpoint("/dev", mountpoint + "/dev", "bind")).mount();
			}
			//errors while mounting any of this partitions may not be a problem
			catch (Gc::Common::SystemException const& e){}
			catch (Gc::Common::MountpointNotFoundException const& e){}
		}
		else
			throw Gc::Common::MissingFstabException("fstab is required but was not found", __FILE__, __LINE__);
		this->loaded = true;
	}

	public: Gc::Model::Device::MountTableMountpoint& findByDevice(std::string device)
	{
		for (std::list<Gc::Model::Device::MountTableMountpoint>::iterator iter = this->begin(); iter != this->end(); iter++){
			if (iter->device == device) {
				return *iter;
			}
		}
		throw Gc::Common::ItemNotFoundException("no mountpoint found by device " + device);
	}

	public: Gc::Model::Device::MountTableMountpoint& getByFilePath(std::string path)
	{
		Gc::Model::Device::MountTableMountpoint* result = NULL;
		for (std::list<Gc::Model::Device::MountTableMountpoint>::iterator iter = this->begin(); iter != this->end(); iter++) {
			// look for the longest matching mountpoint because "/" matches as well as "/mnt" if filename is "/mnt/foo.iso"
			if (path.substr(0, iter->mountpoint.size()) == iter->mountpoint && (result == NULL || iter->mountpoint.size() > result->mountpoint.size())) {
				result = &*iter;
			}
		}

		if (result == NULL) {
			throw Gc::Common::ItemNotFoundException("no mountpoint found by path " + path);
		} else {
			return *result;
		}
	}

	public: operator std::string() const
	{
		std::string result;
		for (Gc::Model::Device::MountTable::const_iterator iter = this->begin(); iter != this->end(); iter++){
			result += std::string("[") + (iter->isMounted ? "x" : " ")  + "] " + iter->device + " " + iter->mountpoint + "\n";
		}
		return result;
	}

};}}}

#endif

