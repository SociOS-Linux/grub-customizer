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
	GrubConfUIGtk ui(config);
	config.connectUI(ui);

	ui.run();
}







