#ifndef PARTITIONCHOOSER_H_
#define PARTITIONCHOOSER_H_

#include "evt_partitionChooser.h"
/**
 * Interface to be implemented by partition chooser dialogs
 */
class PartitionChooser {
public:
	enum Exception {
		ERR_CHKBUTTON_NOT_FOUND
	};
	enum MountExceptionType {
		MOUNT_FAILED,
		UMOUNT_FAILED,
		MOUNT_ERR_NO_FSTAB,
		SUB_MOUNT_FAILED,
		SUB_UMOUNT_FAILED
	};
	//assigns the event listener
	virtual void setEventListener(EventListener_partitionChooser& eventListener)=0;

	//reads the selected root device
	virtual std::string getSelectedDevice()=0;
	//add item to the end of the partition selector
	virtual void addPartitionSelectorItem(Glib::ustring const& device, Glib::ustring const& type, Glib::ustring const& label)=0;
	//remove all items from the partition selector
	virtual void clearPartitionSelector()=0;
	//add submountpoint to the end of the existing list
	virtual void addSubmountpoint(std::string const& mountpoint, bool isMounted)=0;
	//remove all existing submountpoint items
	virtual void removeAllSubmountpoints()=0;
	//show error message (type=what should be showed, PartitionChooser decides which text should be displayed)
	virtual void showErrorMessage(MountExceptionType type)=0;
	//hide the dialog
	virtual void hide()=0;
	//show the dialog, but don't wail until it has been closed/hidden
	virtual void show()=0;
	//sets whether a submountpoint is activated (mounted)
	virtual void setSubmountpointSelectionState(Glib::ustring const& submountpoint, bool new_isSelected)=0;
	//sets whether there's already a mounted root partition
	virtual void setIsMounted(bool isMounted)=0;
	//sets whether this dialog has been started using a live cd
	virtual void setIsStartedManually(bool val)=0;
};

#endif
