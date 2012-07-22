#ifndef GRUBCONF_UI_H_INCLUDED
#define GRUBCONF_UI_H_INCLUDED

class GrubConfUI {
	public:
	virtual void event_load_progress_changed()=0;
	virtual void event_save_progress_changed()=0;
	virtual void event_thread_died()=0;
	virtual void event_grub_install_ready()=0;
};

#endif
