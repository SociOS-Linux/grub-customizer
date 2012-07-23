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
	this->cmd_prefix = dir_prefix != "" ? "chroot '"+dir_prefix+"' " : "";
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
			this->mkdevicemap_cmd = "burg-mkdevicemap --device-map=/dev/stdout";
			this->cfg_dir = dir_prefix+"/etc/burg.d";
			this->cfg_dir_noprefix = "/etc/burg.d";
			this->output_config_dir =  dir_prefix+"/boot/burg";
			this->output_config_file = dir_prefix+"/boot/burg/burg.cfg";
			this->settings_file = dir_prefix+"/etc/default/burg";
			this->devicemap_file = dir_prefix+"/boot/burg/device.map";
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
			this->mkdevicemap_cmd = "grub-mkdevicemap --device-map=/dev/stdout";
			this->cfg_dir = dir_prefix+"/etc/grub.d";
			this->cfg_dir_noprefix = "/etc/grub.d";
			this->output_config_dir =  dir_prefix+"/boot/grub";
			this->output_config_file = dir_prefix+"/boot/grub/grub.cfg";
			this->settings_file = dir_prefix+"/etc/default/grub";
			this->devicemap_file = dir_prefix+"/boot/grub/device.map";
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
	this->mkdevicemap_cmd = cmd_prefix+this->mkdevicemap_cmd;
	
	return is_valid;
}

void GrubEnv::loadFromFile(FILE* cfg_file, std::string const& dir_prefix) {
	SettingsStore ds(cfg_file);
	this->mkconfig_cmd = ds.getValue("MKCONFIG_CMD");
	this->install_cmd = ds.getValue("INSTALL_CMD");
	this->mkfont_cmd = ds.getValue("MKFONT_CMD");
	this->mkdevicemap_cmd = ds.getValue("MKDEVICEMAP_CMD");
	this->cfg_dir = dir_prefix + ds.getValue("CFG_DIR");
	this->cfg_dir_noprefix = ds.getValue("CFG_DIR");
	this->output_config_dir =  dir_prefix + ds.getValue("OUTPUT_DIR");
	this->output_config_file = dir_prefix + ds.getValue("OUTPUT_FILE");
	this->settings_file = dir_prefix + ds.getValue("SETTINGS_FILE");
	this->devicemap_file = dir_prefix + ds.getValue("DEVICEMAP_FILE");
}

void GrubEnv::save() {
	FILE* cfg_file = NULL;
	DIR* dir = opendir((cfg_dir_prefix + "/etc/grub-customizer").c_str());
	if (dir) {
		closedir(dir);
	} else {
		int res = mkdir((cfg_dir_prefix + "/etc/grub-customizer").c_str(), 0755);
		if (res != 0) {
			throw FILE_SAVE_FAILED;
		}
	}
	if (this->burgMode) {
		cfg_file = fopen((cfg_dir_prefix + "/etc/grub-customizer/burg.cfg").c_str(), "w");
	} else {
		cfg_file = fopen((cfg_dir_prefix + "/etc/grub-customizer/grub.cfg").c_str(), "w");
	}
	if (!cfg_file) {
		throw FILE_SAVE_FAILED;
	}
	SettingsStore ds;
	std::map<std::string, std::string> props = this->getProperties();
	for (std::map<std::string, std::string>::iterator iter = props.begin(); iter != props.end(); iter++) {
		ds.setValue(iter->first, iter->second);
	}
	ds.save(cfg_file);
	fclose(cfg_file);
}

std::map<std::string, std::string> GrubEnv::getProperties() {
	std::map<std::string, std::string> result;
	result["MKCONFIG_CMD"] = this->mkconfig_cmd.substr(this->cmd_prefix.size());
	result["UPDATE_CMD"] = this->update_cmd.substr(this->cmd_prefix.size());
	result["INSTALL_CMD"] = this->install_cmd.substr(this->cmd_prefix.size());
	result["MKFONT_CMD"] = this->mkfont_cmd.substr(this->cmd_prefix.size());
	result["MKDEVICEMAP_CMD"] = this->mkdevicemap_cmd.substr(this->cmd_prefix.size());
	result["CFG_DIR"] = this->cfg_dir_noprefix;
	result["OUTPUT_DIR"] = this->output_config_dir.substr(this->cfg_dir_prefix.size());
	result["OUTPUT_FILE"] = this->output_config_file.substr(this->cfg_dir_prefix.size());
	result["SETTINGS_FILE"] = this->settings_file.substr(this->cfg_dir_prefix.size());
	result["DEVICEMAP_FILE"] = this->devicemap_file.substr(this->cfg_dir_prefix.size());

	return result;
}

void GrubEnv::setProperties(std::map<std::string, std::string> const& props) {
	this->mkconfig_cmd = this->cmd_prefix + props.at("MKCONFIG_CMD");
	this->update_cmd = this->cmd_prefix + props.at("UPDATE_CMD");
	this->install_cmd = this->cmd_prefix + props.at("INSTALL_CMD");
	this->mkfont_cmd = this->cmd_prefix + props.at("MKFONT_CMD");
	this->mkdevicemap_cmd = this->cmd_prefix + props.at("MKDEVICEMAP_CMD");
	this->cfg_dir_noprefix = props.at("CFG_DIR");
	this->cfg_dir = this->cfg_dir_prefix + props.at("CFG_DIR");
	this->output_config_dir = this->cfg_dir_prefix + props.at("OUTPUT_DIR");
	this->output_config_file = this->cfg_dir_prefix + props.at("OUTPUT_FILE");
	this->settings_file = this->cfg_dir_prefix + props.at("SETTINGS_FILE");
	this->devicemap_file = this->cfg_dir_prefix + props.at("DEVICEMAP_FILE");
}

std::list<std::string> GrubEnv::getRequiredProperties() {
	std::list<std::string> result;
	result.push_back("MKCONFIG_CMD");
	result.push_back("UPDATE_CMD");
	result.push_back("INSTALL_CMD");
	result.push_back("CFG_DIR");
	return result;
}

std::list<std::string> GrubEnv::getValidProperties() {
	std::list<std::string> result;
	if (this->check_cmd(this->mkconfig_cmd.substr(this->cmd_prefix.size()), this->cmd_prefix)) {
		result.push_back("MKCONFIG_CMD");
	}
	if (this->check_cmd(this->update_cmd.substr(this->cmd_prefix.size()), this->cmd_prefix)) {
		result.push_back("UPDATE_CMD");
	}
	if (this->check_cmd(this->install_cmd.substr(this->cmd_prefix.size()), this->cmd_prefix)) {
		result.push_back("INSTALL_CMD");
	}
	if (this->check_cmd(this->mkfont_cmd.substr(this->cmd_prefix.size()), this->cmd_prefix)) {
		result.push_back("MKFONT_CMD");
	}
	if (this->check_cmd(this->mkdevicemap_cmd.substr(this->cmd_prefix.size()), this->cmd_prefix)) {
		result.push_back("MKDEVICEMAP_CMD");
	}
	if (this->check_dir(this->cfg_dir)) {
		result.push_back("CFG_DIR");
	}
	if (this->check_dir(this->output_config_dir)) {
		result.push_back("OUTPUT_DIR");
	}
	if (this->check_file(this->output_config_file)) {
		result.push_back("OUTPUT_FILE");
	}
	if (this->check_file(this->settings_file)) {
		result.push_back("SETTINGS_FILE");
	}
	if (this->check_file(this->devicemap_file)) {
		result.push_back("DEVICEMAP_FILE");
	}
	return result;
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

bool GrubEnv::check_file(std::string const& file_str) const {
	FILE* file = fopen(file_str.c_str(), "r");
	if (file){
		fclose(file);
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

