#ifndef GRUB_INSTALL_DLG_GTK_INCLUDED
#define GRUB_INSTALL_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include "../interface/evt_grubInstallDlg.h"
#include <libintl.h>
#include "../interface/grubInstallDlg.h"
class GrubInstallDlgGtk : public Gtk::Dialog, public GrubInstallDlg {
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
	GrubInstallDlgGtk();
	void show();
	void showMessageGrubInstallCompleted(std::string const& msg);
	void setEventListener(EventListener_grubInstallDlg& eventListener);
};
#endif
