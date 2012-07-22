#ifndef GRUB_INSTALL_DLG_INCLUDED
#define GRUB_INSTALL_DLG_INCLUDED
#include <gtkmm.h>
#include "../interface/evt_grubInstallDlg.h"
#include <libintl.h>
class GrubInstallDlg : public Gtk::Dialog {
	Gtk::Label lblDescription;
	Gtk::HBox hbDevice;
	Gtk::Label lblDevice, lblInstallInfo;
	Gtk::Entry txtDevice;
	Glib::Dispatcher disp_grub_install_ready;
	EventListener_grubInstallDlg* eventListener;
	void func_disp_grub_install_ready();
	void signal_grub_install_dialog_response(int response_id);
	Glib::ustring install_result;
	public:
	GrubInstallDlg();
	void show();
	void showMessageGrubInstallCompleted(std::string const& msg);
	void setEventListener(EventListener_grubInstallDlg& eventListener);
};
#endif
