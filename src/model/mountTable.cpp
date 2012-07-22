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

#include "mountTable.h"

Mountpoint::Mountpoint(std::string const& mountpoint, bool isMounted) : isMounted(isMounted), mountpoint(mountpoint) {}

Mountpoint::Mountpoint(std::string const& device, std::string const& mountpoint, std::string const& options, bool isMounted)
	: device(device), mountpoint(mountpoint), isMounted(isMounted), options(options)
{
}

bool Mountpoint::isValid(std::string const& prefix, bool isRoot) const {
	return device != "" && (mountpoint != prefix || isRoot) && mountpoint != prefix+"none" && fileSystem != "" && options != "" && dump != "" && pass != "";
}
Mountpoint::operator bool() const {
	return this->isValid();
}

void Mountpoint::mount(){
	if (!isMounted){
		int res = system(("mount '"+device+"' '"+mountpoint+"'"+(options != "" ? " -o '"+options+"'" : "")).c_str());
		if (res != 0)
			throw MOUNT_FAILED;

		this->isMounted = true;
	}
}
void Mountpoint::umount(){
	if (isMounted){
		int res = system(("umount '"+mountpoint+"'").c_str());
		if (res != 0)
			throw UMOUNT_FAILED;

		this->isMounted = false;
	}
}

bool Mountpoint::isLiveCdFs(){
	return this->fileSystem == "aufs";
}

MountTable::MountTable(FILE* source, std::string const& prefix, bool default_isMounted_flag){
	this->loadData(source, prefix, default_isMounted_flag);
}

MountTable::MountTable(std::string const& rootDirectory){
	this->loadData(rootDirectory);
}

MountTable::MountTable(){}

void MountTable::loadData(FILE* source, std::string const& prefix, bool default_isMounted_flag){
	int c;
	int rowEntryPos = 0;
	bool isComment = false;
	bool isBeginOfRow = true;
	char previousChar = 0;
	Mountpoint newMp(prefix, default_isMounted_flag);
	while ((c = fgetc(source)) != EOF){
		if (isBeginOfRow && c == '#')
			isComment = true;
		else if (c == '\n'){
			bool isRoot = newMp.mountpoint == prefix + "/";
			if (newMp.mountpoint[newMp.mountpoint.length()-1] == '/')
				newMp.mountpoint = newMp.mountpoint.substr(0, newMp.mountpoint.length()-1);

			if (newMp.isValid(prefix, isRoot)){
				this->remove(newMp);
				this->push_back(newMp);
			}

			newMp = Mountpoint(prefix, default_isMounted_flag);
			rowEntryPos = 0;
			isBeginOfRow = true;
			isComment = false;
		}
		else if (!isComment) {
			if (c == ' ' || c == '\t'){
				if (previousChar != ' ' && previousChar != '\t')
					rowEntryPos++;
				isBeginOfRow = false;
			}
			else {
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
	if (newMp.mountpoint[newMp.mountpoint.length()-1] == '/')
		newMp.mountpoint = newMp.mountpoint.substr(0, newMp.mountpoint.length()-1);

	if (newMp.isValid(prefix)){
		this->remove(newMp);
		this->push_back(newMp);
	}

	loaded = true;
}

void MountTable::loadData(std::string const& rootDirectory){
	FILE* fstabFile = fopen((rootDirectory+"/etc/fstab").c_str(), "r");
	if (fstabFile != NULL){
		this->loadData(fstabFile, rootDirectory);
		MountTable mtab;
		FILE* mtabfile = fopen("/etc/mtab", "r"); //use global mtab - the local one is unmanaged
		if (mtabfile){
			mtab = MountTable(mtabfile, "", true);
			fclose(mtabfile);
		}
		this->sync(mtab);
		fclose(fstabFile);
	}
}

void MountTable::clear(std::string const& prefix){
	MountTable::iterator iter = this->begin();
	while (iter != this->end()){
		if (iter->mountpoint.substr(0, iter->mountpoint.length()) == prefix){
			this->erase(iter);
			iter = this->begin();
		}
		else
			iter++;
	}
	loaded = false;
}

void MountTable::sync(MountTable const& mtab){
	for (MountTable::const_iterator iter = mtab.begin(); iter != mtab.end(); iter++){
		this->add(*iter);
	}
}

bool MountTable::isLoaded() const {
	return loaded;
}

MountTable::operator bool() const {
	return isLoaded();
}

Mountpoint& MountTable::getEntryRefByMountpoint(std::string const& mountPoint) {
	for (std::list<Mountpoint>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->mountpoint == mountPoint)
			return *iter;
	}
	throw MOUNTPOINT_NOT_FOUND;
}

Mountpoint MountTable::getEntryByMountpoint(std::string const& mountPoint) const {
	for (std::list<Mountpoint>::const_iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->mountpoint == mountPoint)
			return *iter;
	}
	return Mountpoint();
}

Mountpoint& MountTable::add(Mountpoint const& mpToAdd){
	this->remove(mpToAdd); //remove existing mountpoints with the same directory
	this->push_back(mpToAdd);
	return this->back();
}

void MountTable::remove(Mountpoint const& mountpoint){
	for (std::list<Mountpoint>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->mountpoint == mountpoint.mountpoint){
			this->erase(iter);
			break;
		}
	}
}

void MountTable::umountAll(std::string const& prefix){
	for (MountTable::reverse_iterator iter = this->rbegin(); iter != this->rend(); iter++){
		if (iter->mountpoint.substr(0, prefix.length()) == prefix && iter->isMounted){
			iter->umount();
		}
	}
}

void MountTable::mountRootFs(std::string const& device, std::string const& mountpoint){
	this->add(Mountpoint(device, mountpoint, "")).mount();
	this->loadData(mountpoint);
	FILE* fstab = fopen((mountpoint + "/etc/fstab").c_str(), "r");
	if (fstab){
		fclose(fstab); //opening of fstab is just a test

		try {
			this->add(Mountpoint("/proc", mountpoint + "/proc", "bind")).mount();
			this->add(Mountpoint("/sys", mountpoint + "/sys", "bind")).mount();
			this->add(Mountpoint("/dev", mountpoint + "/dev", "bind")).mount();
		}
		//errors while mounting any of this partitions may not be a problem
		catch (MountTable::Exception e){}
		catch (Mountpoint::Exception e){}
	}
	else
		throw MOUNT_ERR_NO_FSTAB;
	this->loaded = true;
}

MountTable::operator std::string() const {
	std::string result;
	for (MountTable::const_iterator iter = this->begin(); iter != this->end(); iter++){
		result += std::string("[") + (iter->isMounted ? "x" : " ")  + "] " + iter->device + " " + iter->mountpoint + "\n";
	}
	return result;
}
