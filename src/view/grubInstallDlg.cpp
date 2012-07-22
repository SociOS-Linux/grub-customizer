#include "grubInstallDlg.h"

GrubInstallDlg::GrubInstallDlg()
	: lblDescription(gettext("Install the bootloader to MBR and put some\nfiles to the bootloaders data directory\n(if they don't already exist)."), Gtk::ALIGN_LEFT)
	, lblDevice(gettext("_Device: "), Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, true)
{
	Gtk::VBox* vbDialog = this->get_vbox();
	this->set_icon_name("grub-customizer");
	vbDialog->pack_start(lblDescription, Gtk::PACK_SHRINK);
	vbDialog->pack_start(hbDevice);
	vbDialog->pack_start(lblInstallInfo);
	hbDevice.pack_start(lblDevice, Gtk::PACK_SHRINK);
	hbDevice.pack_start(txtDevice);
	txtDevice.set_text("/dev/sda");
	this->set_title(gettext("Install to MBR"));
	vbDialog->set_spacing(5);
	lblDevice.set_mnemonic_widget(txtDevice);
	this->set_border_width(5);
	this->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	this->add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	this->set_default_response(Gtk::RESPONSE_OK);
	txtDevice.set_activates_default(true);

	this->signal_response().connect(sigc::mem_fun(this, &GrubInstallDlg::signal_grub_install_dialog_response));
	
	disp_grub_install_ready.connect(sigc::mem_fun(this, &GrubInstallDlg::func_disp_grub_install_ready));
}

void GrubInstallDlg::func_disp_grub_install_ready(){
	std::string output = this->install_result;
	if (output == ""){
		Gtk::MessageDialog msg(gettext("The bootloader has been installed successfully"));
		msg.run();
		this->hide();
	}
	else {
		Gtk::MessageDialog msg(gettext("Error while installing the bootloader"), false, Gtk::MESSAGE_ERROR);
		msg.set_secondary_text(output);
		msg.run();
	}
	this->set_response_sensitive(Gtk::RESPONSE_OK, true);
	this->set_response_sensitive(Gtk::RESPONSE_CANCEL, true);
	txtDevice.set_sensitive(true);
	lblInstallInfo.set_text("");
}

void GrubInstallDlg::signal_grub_install_dialog_response(int response_id){
	if (response_id == Gtk::RESPONSE_OK){
		if (txtDevice.get_text().length()){
			this->set_response_sensitive(Gtk::RESPONSE_OK, false);
			this->set_response_sensitive(Gtk::RESPONSE_CANCEL, false);
			txtDevice.set_sensitive(false);
			lblInstallInfo.set_text(gettext("installing the bootloader…"));
			
			eventListener->installGrub_request(txtDevice.get_text());
		}
		else
			Gtk::MessageDialog(gettext("Please type a device string!")).run();
	}
	else
		this->hide();
}

void GrubInstallDlg::showMessageGrubInstallCompleted(std::string const& msg){
	this->install_result = msg;
	disp_grub_install_ready();
}

void GrubInstallDlg::setEventListener(EventListenerView_iface& eventListener) {
	this->eventListener = &eventListener;
}

void GrubInstallDlg::show(){
	this->show_all();
}


