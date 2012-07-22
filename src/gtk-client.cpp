#include "grubconfig.h"
#include "grubconf_ui_gtk.h"
#include <libintl.h>
#include <locale.h>
#include "config.h"

#include "mountTable.h"
#include "liveCDSetupDialog.h"

bool checkcmd(std::string const& cmd, bool show_error = true, std::string cmd_prefix = ""){
	std::cout << "checking for the " << cmd << " command… " << std::endl;
	int res = system((cmd_prefix+" which "+cmd).c_str());
	if (res != 0)
		Gtk::MessageDialog(cmd+gettext(" command not found, cannot proceed"), false, Gtk::MESSAGE_ERROR).run();
	return res == 0;
}

int main(int argc, char** argv){
	setlocale( LC_ALL, "");
	bindtextdomain( "grub-customizer", LOCALEDIR);
	textdomain( "grub-customizer" );

	Gtk::Main app(argc, argv);
	Glib::thread_init();

	GrubConfig config;
	
	if (config.cfgDirIsClean() == false)
		config.cleanupCfgDir();
	GrubConfUIGtk ui(config);
	config.connectUI(ui);
	bool do_continue = config.prepare();
	if (do_continue){
		ui.run();
	}
	config.umountSwitchedRootPartition(); //cleanup… only if another partition has been mounted
}







