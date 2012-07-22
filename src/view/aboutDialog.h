#ifndef GC_ABOUNTDIALOG_INCLUDED
#define GC_ABOUNTDIALOG_INCLUDED
#include <gtkmm.h>
#include "../config.h"
#include <libintl.h>
class AboutDialog : public Gtk::AboutDialog {
	Glib::ustring appName, appVersion;
	std::vector<Glib::ustring> authors;

	void signal_about_dlg_response(int response_id);
public:
	AboutDialog();
};

#endif
