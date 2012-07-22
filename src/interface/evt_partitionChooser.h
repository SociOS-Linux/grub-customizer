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
