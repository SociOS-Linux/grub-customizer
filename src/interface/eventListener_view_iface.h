#ifndef EVENTLISTENER_VIEW_IFACE
#define EVENTLISTENER_VIEW_IFACE
#include <string>

class EventListenerView_iface {
	public:
	virtual void settings_dialog_request()=0;
	virtual void reload_request()=0;
	virtual void rootSelectorCompleted()=0;
	virtual void save_request()=0;
	virtual void rootSelector_request()=0;
	virtual void installDialogRequest()=0;
	virtual void installGrub_request(std::string const& device)=0;
	virtual void scriptAddDlg_requested()=0;
	virtual void scriptAddDlg_applied()=0;
	virtual void scriptSelected()=0;
	virtual void removeProxy_requested(void* p)=0;
	virtual bool exitRequest()=0;

	virtual void signal_script_state_toggled(void* script)=0;
	virtual void signal_entry_state_toggled(void* entry)=0;
	virtual void signal_entry_renamed(void* entry)=0;
	
	virtual void ruleSwap_requested(void* a, void* b)=0;
	virtual void proxySwap_requested(void* a, void* b)=0;
	
	virtual void ruleSelected(void* rule)=0;
	virtual void proxySelected(void* proxy)=0;

	virtual void aboutDialog_requested()=0;

	//settings dialog
	virtual void setting_row_changed(std::string const& name)=0;
	virtual void default_entry_predefined_toggeled()=0;
	virtual void default_entry_saved_toggeled()=0;
	virtual void default_entry_changed()=0;
	virtual void showMenu_toggled()=0;
	virtual void osProber_toggled()=0;
	virtual void timeout_changed()=0;
	virtual void kernelparams_changed()=0;
	virtual void generateRecovery_toggled()=0;
	virtual void useCustomResolution_toggled()=0;
	virtual void resolution_changed()=0;
	virtual void colorChange_requested()=0;
	virtual void backgroundChange_requested()=0;
	virtual void backgroundCopy_requested()=0;
	virtual void backgroundRemove_requested()=0;
	virtual void settings_dialog_hide_request()=0;

	//root selector
	virtual void partitionChooser_applied()=0;
	virtual void partitionChooser_cancelled()=0;
	virtual void rootFsMount_request()=0;
	virtual void rootFsUmount_request()=0;
	virtual void submountpoint_mount_request(std::string const& mountpoint)=0;
	virtual void submountpoint_umount_request(std::string const& mountpoint)=0;
};

#endif
