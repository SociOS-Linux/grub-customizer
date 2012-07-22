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
	
	FILE* mtabFile = fopen("/etc/mtab", "r");
	MountTable mtab;
	if (mtabFile){
		mtab.loadData(mtabFile);
		fclose(mtabFile);
	}
	
	std::cout << "checking for the which command…" << std::endl;
	int which_exists = system("which which");
	
	
	
	std::string dir_prefix = "", cmd_prefix = "";
	bool livecd_setupdialog_cancelled = false;
	if (mtab && mtab.getEntryByMountpoint("/").fileSystem == "aufs"){ //"aufs" is the file system of live cds
		if (!checkcmd("mount")) return 1;
		if (!checkcmd("umount")) return 1;
		if (!checkcmd("chroot")) return 1;

		LiveCDSetupDialog setupDialog;
		setupDialog.set_icon_name("grub-customizer");
		setupDialog.readPartitionInfo();
		setupDialog.show();
		Gtk::Main::run(setupDialog);
		if (setupDialog.isCancelled())
			livecd_setupdialog_cancelled = true;
		dir_prefix = setupDialog.getRootMountpoint();
		cmd_prefix = "chroot '"+dir_prefix+"' ";
	}
	
	if (!livecd_setupdialog_cancelled){ //… if not started, this it false and the following operations work
		DIR* burgdir = opendir((dir_prefix+"/etc/burg.d").c_str());
		bool burg_exists = burgdir && checkcmd("update-burg", false, cmd_prefix) && checkcmd("burg-mkconfig", false, cmd_prefix);
		if (burgdir)
			closedir(burgdir);
		DIR* grubdir = opendir((dir_prefix+"/etc/grub.d").c_str());
		bool burgMode = false;
		if (burg_exists && grubdir){
			Gtk::MessageDialog dlg(gettext("BURG found!"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
			dlg.set_secondary_text(gettext("Do you want to configure BURG instead of grub2?"));
			dlg.set_default_response(Gtk::RESPONSE_YES);
			int result = dlg.run();
			if (result == Gtk::RESPONSE_YES)
				burgMode = true;
			closedir(grubdir);
		}
		else if (burg_exists){
			burgMode = true;
		}
		else if (grubdir){
			closedir(grubdir);
		}
		if (burgMode){
			config.burgMode = true;
			config.mkconfig_cmd = cmd_prefix+"burg-mkconfig";
			config.update_cmd = cmd_prefix+"update-burg";
			config.install_cmd = cmd_prefix+"burg-install";
			config.cfg_dir = dir_prefix+"/etc/burg.d";
			config.cfg_dir_noprefix = "/etc/burg.d";
			config.output_config_file = dir_prefix+"/boot/burg/burg.cfg";
			std::cout << "BURG mode!" << std::endl;
		}
		else {
			if (!checkcmd("grub-mkconfig", true, cmd_prefix)) return 1;
			if (!checkcmd("update-grub", true, cmd_prefix)) return 1;
			if (!checkcmd("grub-install", true, cmd_prefix)) return 1;
			config.burgMode = false;
			config.mkconfig_cmd = cmd_prefix+"grub-mkconfig";
			config.update_cmd = cmd_prefix+"update-grub";
			config.install_cmd = cmd_prefix+"grub-install";
			config.cfg_dir = dir_prefix+"/etc/grub.d";
			config.cfg_dir_noprefix = "/etc/grub.d";
			config.output_config_file = dir_prefix+"/boot/grub/grub.cfg";
			std::cout << "GRUB 2 mode" << std::endl;
		}
	
	
	
		GrubConfUIGtk ui(config);
		config.connectUI(ui);

		ui.run();
	}
	
	if (dir_prefix != ""){
		system(("chroot '"+dir_prefix+"' umount -a").c_str());
		system(("umount '"+dir_prefix+"/dev'").c_str());
		system(("umount '"+dir_prefix+"/sys'").c_str());
		system(("umount '"+dir_prefix+"/proc'").c_str());
		system(("umount '"+dir_prefix+"'").c_str());
	}

}







