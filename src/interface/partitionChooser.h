#ifndef PARTITIONCHOOSER_H_
#define PARTITIONCHOOSER_H_

#include "evt_partitionChooser.h"
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
	virtual void setEventListener(EventListener_partitionChooser& eventListener)=0;
	virtual void updateSensitivity()=0;
	virtual void signal_custom_partition_toggled()=0;
	virtual void signal_lvRootPartitionSelection_changed()=0;
	virtual void signal_custom_partition_typing()=0;
	virtual void signal_btt_mount_click()=0;
	virtual void signal_btt_umount_click()=0;
	virtual void on_cancel()=0;
	virtual void on_apply()=0;

	virtual std::string getSelectedDevice()=0;
	virtual void addPartitionSelectorItem(Glib::ustring const& device, Glib::ustring const& type, Glib::ustring const& label)=0;
	virtual void clearPartitionSelector()=0;
	virtual void addSubmountpoint(std::string const& mountpoint, bool isMounted)=0;
	virtual void removeAllSubmountpoints()=0;
	virtual void showErrorMessage(MountExceptionType type)=0;
	virtual void run()=0;
	virtual void hide()=0;
	virtual void show()=0;
	virtual void setSubmountpointSelectionState(Glib::ustring const& submountpoint, bool new_isSelected)=0;
	virtual void setIsMounted(bool isMounted)=0;
};

#endif
