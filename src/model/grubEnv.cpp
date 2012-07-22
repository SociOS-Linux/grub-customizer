#include "grubEnv.h"

GrubEnv::GrubEnv() : burgMode(false) {}

bool GrubEnv::init(GrubEnv::Mode mode, std::string const& dir_prefix){
	std::string cmd_prefix = dir_prefix != "" ? "chroot '"+dir_prefix+"' " : "";
	this->cfg_dir_prefix = dir_prefix;
	switch (mode){
	case BURG_MODE:
		this->burgMode = true;
		this->mkconfig_cmd = "burg-mkconfig";
		this->update_cmd = "update-burg";
		this->install_cmd = "burg-install";
		this->cfg_dir = dir_prefix+"/etc/burg.d";
		this->cfg_dir_noprefix = "/etc/burg.d";
		this->output_config_dir =  dir_prefix+"/boot/burg";
		this->output_config_file = dir_prefix+"/boot/burg/burg.cfg";
		this->settings_file = dir_prefix+"/etc/default/burg";
		break;
	case GRUB_MODE:
		this->burgMode = false;
		this->mkconfig_cmd = "grub-mkconfig";
		this->update_cmd = "update-grub";
		this->install_cmd = "grub-install";
		this->cfg_dir = dir_prefix+"/etc/grub.d";
		this->cfg_dir_noprefix = "/etc/grub.d";
		this->output_config_dir =  dir_prefix+"/boot/grub";
		this->output_config_file = dir_prefix+"/boot/grub/grub.cfg";
		this->settings_file = dir_prefix+"/etc/default/grub";
		break;
	}
	
	bool is_valid = check_cmd(mkconfig_cmd, cmd_prefix) && check_cmd(update_cmd, cmd_prefix) && check_cmd(install_cmd, cmd_prefix) && check_dir(cfg_dir);
	
	this->mkconfig_cmd = cmd_prefix+this->mkconfig_cmd;
	this->update_cmd = cmd_prefix+this->update_cmd;
	this->install_cmd = cmd_prefix+this->install_cmd;
	
	return is_valid;
}

bool GrubEnv::check_cmd(std::string const& cmd, std::string const& cmd_prefix){
	std::cout << "checking for the " << cmd << " command… " << std::endl;
	int res = system((cmd_prefix+" which "+cmd).c_str());
	return res == 0;
}

bool GrubEnv::check_dir(std::string const& dir_str){
	DIR* dir = opendir(dir_str.c_str());
	if (dir){
		closedir(dir);
		return true;
	}
	return false;
}

bool GrubEnv::isLiveCD(){
	FILE* mtabFile = fopen("/etc/mtab", "r");
	MountTable mtab;
	if (mtabFile){
		mtab.loadData(mtabFile, "");
		fclose(mtabFile);
	}
	return mtab && mtab.getEntryByMountpoint("/").fileSystem == "aufs";
}

std::string GrubEnv::getRootDevice(){
	FILE* mtabFile = fopen("/etc/mtab", "r");
	MountTable mtab;
	if (mtabFile){
		mtab.loadData(mtabFile, "");
		fclose(mtabFile);
	}
	return mtab.getEntryByMountpoint(cfg_dir_prefix == "" ? "/" : cfg_dir_prefix).device;
}



