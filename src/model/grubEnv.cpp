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

bool GrubEnv::check_cmd(std::string const& cmd, std::string const& cmd_prefix) const {
	this->log("checking the " + cmd + " command… ", Logger::INFO);
	FILE* proc = popen((cmd_prefix + " which " + cmd + " 2>&1").c_str(), "r");
	std::string output;
	int c;
	while ((c = fgetc(proc)) != EOF) {
		if (c != '\n') {
			output += char(c);
		}
	}
	this->log("found at: " + output, Logger::INFO);
	return pclose(proc) == 0;
}

bool GrubEnv::check_dir(std::string const& dir_str) const {
	DIR* dir = opendir(dir_str.c_str());
	if (dir){
		closedir(dir);
		return true;
	}
	return false;
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



