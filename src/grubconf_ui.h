#ifndef GRUBCONF_UI_H_INCLUDED
#define GRUBCONF_UI_H_INCLUDED
#include <string>

class GrubConfUI {
	public:
	virtual void event_load_progress_changed()=0;
	virtual void event_save_progress_changed()=0;
	virtual void event_thread_died()=0;
	virtual void event_grub_install_ready()=0;
	virtual bool bootloader_not_found_requestForRootSelection()=0; //return value: answer positive?
	virtual std::string show_root_selector()=0; //return value: root mountpoint, empty if cancelled
	virtual bool requestForBurgMode()=0; //return value: configure burg instead of grub2
	virtual void event_mode_changed()=0;
};

#endif
