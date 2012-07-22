#ifndef EVENTLISTENER_PARTITIONCHOOSER_INCLUDED
#define EVENTLISTENER_PARTITIONCHOOSER_INCLUDED

#include <string>
class EventListener_partitionChooser {
public:
	virtual void partitionChooser_applied()=0;
	virtual void partitionChooser_cancelled()=0;
	virtual void rootFsMount_request()=0;
	virtual void rootFsUmount_request()=0;
	virtual void submountpoint_mount_request(std::string const& mountpoint)=0;
	virtual void submountpoint_umount_request(std::string const& mountpoint)=0;
};

#endif
