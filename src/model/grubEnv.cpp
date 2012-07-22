/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "grubEnv.h"

GrubEnv::GrubEnv() : burgMode(false), useDirectBackgroundProps(false) {}

bool GrubEnv::init(GrubEnv::Mode mode, std::string const& dir_prefix){
	useDirectBackgroundProps = false;
	std::string cmd_prefix = dir_prefix != "" ? "chroot '"+dir_prefix+"' " : "";
	this->cfg_dir_prefix = dir_prefix;
	std::string output_config_file_noprefix;
	switch (mode){
	case BURG_MODE: {
		this->burgMode = true;
		FILE* burg_cfg = fopen((dir_prefix + "/etc/grub-customizer/burg.cfg").c_str(), "r");
		if (burg_cfg) { // try to use the settings file ...
			this->log("using custom BURG configuration", Logger::INFO);
			this->loadFromFile(burg_cfg, dir_prefix);
			fclose(burg_cfg);
		} else { // otherwise use the built-in config
			this->mkconfig_cmd = "burg-mkconfig";
			this->install_cmd = "burg-install";
			this->mkfont_cmd = "burg-mkfont";
			this->cfg_dir = dir_prefix+"/etc/burg.d";
			this->cfg_dir_noprefix = "/etc/burg.d";
			this->output_config_dir =  dir_prefix+"/boot/burg";
			this->output_config_file = dir_prefix+"/boot/burg/burg.cfg";
			this->settings_file = dir_prefix+"/etc/default/burg";
		}
		} break;
	case GRUB_MODE: {
		this->burgMode = false;
		FILE* grub_cfg = fopen((dir_prefix + "/etc/grub-customizer/grub.cfg").c_str(), "r");
		if (grub_cfg) { // try to use the settings file ...
			this->log("using custom Grub2 configuration", Logger::INFO);
			this->loadFromFile(grub_cfg, dir_prefix);
			fclose(grub_cfg);
		} else { // otherwise use the built-in config
			this->mkconfig_cmd = "grub-mkconfig";
			this->install_cmd = "grub-install";
			this->mkfont_cmd = "grub-mkfont";
			this->cfg_dir = dir_prefix+"/etc/grub.d";
			this->cfg_dir_noprefix = "/etc/grub.d";
			this->output_config_dir =  dir_prefix+"/boot/grub";
			this->output_config_file = dir_prefix+"/boot/grub/grub.cfg";
			this->settings_file = dir_prefix+"/etc/default/grub";
		}
		} break;
	}
	
	this->update_cmd = this->mkconfig_cmd + " -o \"" + this->output_config_file.substr(dir_prefix.size()) + "\"";
	this->log("update-CMD: " + this->update_cmd, Logger::INFO);

	bool is_valid = check_cmd(mkconfig_cmd, cmd_prefix) && check_cmd(update_cmd, cmd_prefix) && check_cmd(install_cmd, cmd_prefix) && check_dir(cfg_dir);
	
	this->mkconfig_cmd = cmd_prefix+this->mkconfig_cmd;
	this->update_cmd = cmd_prefix+this->update_cmd;
	this->install_cmd = cmd_prefix+this->install_cmd;
	this->mkfont_cmd = cmd_prefix+this->mkfont_cmd;
	
	return is_valid;
}

void GrubEnv::loadFromFile(FILE* cfg_file, std::string const& dir_prefix) {
	SettingsStore ds(cfg_file);
	this->mkconfig_cmd = ds.getValue("MKCONFIG_CMD");
	this->install_cmd = ds.getValue("INSTALL_CMD");
	this->mkfont_cmd = ds.getValue("MKFONT_CMD");
	this->cfg_dir = dir_prefix + ds.getValue("CFG_DIR");
	this->cfg_dir_noprefix = ds.getValue("CFG_DIR");
	this->output_config_dir =  dir_prefix + ds.getValue("OUTPUT_DIR");
	this->output_config_file = dir_prefix + ds.getValue("OUTPUT_FILE");
	this->settings_file = dir_prefix + ds.getValue("SETTINGS_FILE");
}

bool GrubEnv::check_cmd(std::string const& cmd, std::string const& cmd_prefix) const {
	this->log("checking the " + this->trim_cmd(cmd) + " command… ", Logger::INFO);
	FILE* proc = popen((cmd_prefix + " which " + this->trim_cmd(cmd) + " 2>&1").c_str(), "r");
	std::string output;
	int c;
	while ((c = fgetc(proc)) != EOF) {
		if (c != '\n') {
			output += char(c);
		}
	}
	bool result = pclose(proc) == 0;
	if (result == true) {
		this->log("found at: " + output, Logger::INFO);
	} else {
		this->log("not found", Logger::INFO);
	}
	return result;
}

bool GrubEnv::check_dir(std::string const& dir_str) const {
	DIR* dir = opendir(dir_str.c_str());
	if (dir){
		closedir(dir);
		return true;
	}
	return false;
}

std::string GrubEnv::trim_cmd(std::string const& cmd) const {
	int firstSpace = cmd.find_first_of(' ');
	if (firstSpace != -1) {
		return cmd.substr(0, firstSpace);
	} else {
		return cmd;
	}
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

std::list<GrubEnv::Mode> GrubEnv::getAvailableModes(){
	std::list<Mode> result;
	if (this->init(GrubEnv::BURG_MODE, this->cfg_dir_prefix))
		result.push_back(GrubEnv::BURG_MODE);
	if (this->init(GrubEnv::GRUB_MODE, this->cfg_dir_prefix))
		result.push_back(GrubEnv::GRUB_MODE);
	return result;
}



