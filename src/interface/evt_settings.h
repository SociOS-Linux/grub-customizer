#ifndef EVENTLISTENER_SETTINGSDLG_INCLUDED
#define EVENTLISTENER_SETTINGSDLG_INCLUDED

#include <string>
class EventListener_settings {
public:
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
};

#endif
