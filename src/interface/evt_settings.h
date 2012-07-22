#ifndef EVENTLISTENER_SETTINGSDLG_INCLUDED
#define EVENTLISTENER_SETTINGSDLG_INCLUDED

#include <string>
/**
 * base class to be implemented by event listeners for setting dialogs
 */
class EventListener_settings {
public:
	//generic changes (name, value, is_active)
	virtual void setting_row_changed(std::string const& name)=0;
	//user wants to set a fix default entry
	virtual void default_entry_predefined_toggeled()=0;
	//user wants to use the entries which has been booted before
	virtual void default_entry_saved_toggeled()=0;
	//user has set a fix default entry
	virtual void default_entry_changed()=0;
	//user has toggled the show/hide preference
	virtual void showMenu_toggled()=0;
	//user has decided if os-prober should be activated or not
	virtual void osProber_toggled()=0;
	//user has changed the timeout
	virtual void timeout_changed()=0;
	//user has changes the kernel parameters
	virtual void kernelparams_changed()=0;
	//user has toggled the option to show or hide recovery entries
	virtual void generateRecovery_toggled()=0;
	//user has toggled the option to use or use not a custom resolution (GMX_MODE)
	virtual void useCustomResolution_toggled()=0;
	//user has changed the menu resolution
	virtual void resolution_changed()=0;
	//user has changed one of the menu colors
	virtual void colorChange_requested()=0;
	//user has selected another grub wallpaper
	virtual void backgroundChange_requested()=0;
	//user wants to copy the image into the grub directory
	virtual void backgroundCopy_requested()=0;
	//user wants to remove the selected background image
	virtual void backgroundRemove_requested()=0;
	//user wants to hide the settings dialog
	virtual void settings_dialog_hide_request()=0;
};

#endif
