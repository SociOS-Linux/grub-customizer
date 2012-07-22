#ifndef GRUB_INSTALL_DLG_INCLUDED
#define GRUB_INSTALL_DLG_INCLUDED
#include <gtkmm.h>
#include "../interface/eventListener_view_iface.h"
#include <libintl.h>
class GrubInstallDlg : public Gtk::Dialog {
	Gtk::Label lblDescription;
	Gtk::HBox hbDevice;
	Gtk::Label lblDevice, lblInstallInfo;
	Gtk::Entry txtDevice;
	Glib::Dispatcher disp_grub_install_ready;
	EventListenerView_iface* eventListener;
	void func_disp_grub_install_ready();
	void signal_grub_install_dialog_response(int response_id);
	Glib::ustring install_result;
	public:
	GrubInstallDlg();
	void show();
	void showMessageGrubInstallCompleted(std::string const& msg);
	void setEventListener(EventListenerView_iface& eventListener);
};
#endif
