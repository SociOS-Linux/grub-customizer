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

#ifndef SRC_MODEL_DEVICE_MOUNTTABLEMOUNTPOINT_HPP_
#define SRC_MODEL_DEVICE_MOUNTTABLEMOUNTPOINT_HPP_

namespace Gc { namespace Model { namespace Device { class MountTableMountpoint
{
	public: std::string device, mountpoint, fileSystem, options, dump, pass;

	public: bool isMounted;

	public: bool isValid(std::string const& prefix = "", bool isRoot = false) const
	{
		return device != "" && (mountpoint != prefix || isRoot) && mountpoint != prefix+"none" && fileSystem != "" && options != "" && dump != "" && pass != "";
	}

	public: operator bool() const
	{
		return this->isValid();
	}

	public: MountTableMountpoint(std::string const& mountpoint = "", bool isMounted = false)
		: isMounted(isMounted), mountpoint(mountpoint)
	{}
	
	public: MountTableMountpoint(std::string const& device, std::string const& mountpoint, std::string const& options, bool isMounted = false)
		: device(device), mountpoint(mountpoint), isMounted(isMounted), options(options)
	{
	}

	public: void mount()
	{
		if (!isMounted){
			int res = system(("mount '"+device+"' '"+mountpoint+"'"+(options != "" ? " -o '"+options+"'" : "")).c_str());
			if (res != 0)
				throw Gc::Common::Exception::MountException("mount failed", __FILE__, __LINE__);
	
			this->isMounted = true;
		}
	}

	public: void umount()
	{
		if (isMounted){
			int res = system(("umount '"+mountpoint+"'").c_str());
			if (res != 0)
				throw Gc::Common::Exception::UMountException("umount failed", __FILE__, __LINE__);
	
			this->isMounted = false;
		}
	}

	public: bool isLiveCdFs()
	{
		return this->fileSystem == "aufs" | this->fileSystem == "overlayfs";
	}

};}}}



#endif /* SRC_MODEL_DEVICE_MOUNTTABLEMOUNTPOINT_HPP_ */
