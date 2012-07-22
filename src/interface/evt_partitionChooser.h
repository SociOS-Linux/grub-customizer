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

#ifndef EVENTLISTENER_PARTITIONCHOOSER_INCLUDED
#define EVENTLISTENER_PARTITIONCHOOSER_INCLUDED

#include <string>

/**
 * base class to be implemented by partition chooser event listeners
 */
class EventListener_partitionChooser {
public:
	//user has applied everything he has done using the partition chooser
	virtual void partitionChooser_applied()=0;
	//user has cancelled the partition chooser
	virtual void partitionChooser_cancelled()=0;
	//user has clicked the button to mount the selected partition
	virtual void rootFsMount_request()=0;
	//user has clicked the button to unmount the selected partition
	virtual void rootFsUmount_request()=0;
	//user has clicked the button to mount the selected submountpoint
	virtual void submountpoint_mount_request(std::string const& mountpoint)=0;
	//user has clicked the button to unmount the selected submountpoint
	virtual void submountpoint_umount_request(std::string const& mountpoint)=0;
};

#endif
