#include "grubconfig.h"
#include "grubconf_ui_gtk.h"
#include <libintl.h>
#include <locale.h>
#include "config.h"

int main(int argc, char** argv){
	setlocale( LC_ALL, "");
	bindtextdomain( "grub-customizer", LOCALEDIR);
	textdomain( "grub-customizer" );

	Gtk::Main app(argc, argv);
	Glib::thread_init();

	GrubConfig config;

	DIR* burgdir = opendir("/etc/burg.d");
	bool burgMode = false;
	if (burgdir != NULL){
		Gtk::MessageDialog dlg(gettext("BURG found!"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
		dlg.set_secondary_text(gettext("Do you want to configure BURG instead of grub2?"));
		dlg.set_default_response(Gtk::RESPONSE_YES);
		int result = dlg.run();
		if (result == Gtk::RESPONSE_YES)
			burgMode = true;
		closedir(burgdir);
	}
	if (burgMode){
		config.burgMode = true;
		config.mkconfig_cmd = "burg-mkconfig";
		config.update_cmd = "update-burg";
		config.cfg_dir = "/etc/burg.d";
		std::cout << "BURG mode!" << std::endl;
	}
	else {
		config.burgMode = false;
		config.mkconfig_cmd = "grub-mkconfig";
		config.update_cmd = "update-grub";
		config.cfg_dir = "/etc/grub.d";
		std::cout << "GRUB 2 mode" << std::endl;
	}
	
	
	
	GrubConfUIGtk ui(config);
	config.connectUI(ui);

	ui.run();
}







