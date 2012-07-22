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

#include "settings_mng_ds.h"

SettingsManagerDataStore::SettingsManagerDataStore(GrubEnv& env)
	: _reloadRequired(false), env(env), color_helper_required(false)
{
}

bool SettingsManagerDataStore::reloadRequired() const {
	return this->_reloadRequired;
}

bool SettingsManagerDataStore::load(){
	settings.clear();

	FILE* file = fopen(this->env.settings_file.c_str(), "r");
	if (file){
		SettingsStore::load(file);

		fclose(file);
		return true;
	}
	else
		return false;
}

bool SettingsManagerDataStore::save(){
	const char* background_script = "\
#! /bin/sh -e\n\
# Name of this script: 'grub_background.sh'\n\
\n\
   WALLPAPER=\"/usr/share/images/desktop-base/background\"\n\
   COLOR_NORMAL=\"light-gray/black\"\n\
   COLOR_HIGHLIGHT=\"magenta/black\"\n\
\n\
if [ \"${GRUB_MENU_PICTURE}\" ] ; then\n\
   echo \"using custom appearance settings\" >&2\n\
   [ \"${GRUB_COLOR_NORMAL}\" ] && COLOR_NORMAL=\"${GRUB_COLOR_NORMAL}\"\n\
   [ \"${GRUB_COLOR_HIGHLIGHT}\" ] && COLOR_HIGHLIGHT=\"${GRUB_COLOR_HIGHLIGHT}\"\n\
   WALLPAPER=\"${GRUB_MENU_PICTURE}\"\n\
fi\n";

	FILE* outFile = fopen(this->env.settings_file.c_str(), "w");
	if (outFile){
		bool background_script_required = false;
		bool isGraphical = false;
		this->color_helper_required = false;
		for (std::list<SettingRow>::iterator iter = this->begin(false); iter != this->end(); iter++){
			if (iter != this->begin(false)) {
				fputs("\n", outFile);
			}
			fputs((iter->getOutput()).c_str(), outFile);

			if (!background_script_required && !this->env.useDirectBackgroundProps && (iter->name == "GRUB_MENU_PICTURE" || iter->name == "GRUB_COLOR_NORMAL" || iter->name == "GRUB_COLOR_HIGHLIGHT")) {
				background_script_required = true;
				isGraphical = true;
			}
			if (this->env.useDirectBackgroundProps && (iter->name == "GRUB_COLOR_NORMAL" || iter->name == "GRUB_COLOR_HIGHLIGHT")) {
				this->color_helper_required = true;
				isGraphical = true;
			}
			if (iter->name == "GRUB_BACKGROUND" && this->env.useDirectBackgroundProps) {
				isGraphical = true;
			}
		}
		fclose(outFile);
		if (background_script_required){
			mkdir((env.cfg_dir_prefix+"/usr/share/desktop-base").c_str(), 0755);
			FILE* bgScriptFile = fopen((env.cfg_dir_prefix+"/usr/share/desktop-base/grub_background.sh").c_str(), "w");
			chmod((env.cfg_dir_prefix+"/usr/share/desktop-base/grub_background.sh").c_str(), 0755);
			fputs(background_script, bgScriptFile);
			fclose(bgScriptFile);
		}

		if (isGraphical) {
			FILE* fontFile = fopen((this->env.output_config_dir + "/unicode.pf2").c_str(), "r");
			if (fontFile) {
				this->log("font file exists", Logger::INFO);
				fclose(fontFile);
			} else {
				this->log("generating the font file", Logger::EVENT);
				FILE* mkfont_proc = popen((this->env.mkfont_cmd + " -o " + this->env.output_config_dir + "/unicode.pf2 /usr/share/fonts/dejavu/DejaVuSansMono.ttf 2>&1").c_str(), "r");
				int c;
				std::string row = "";
				while ((c = fgetc(mkfont_proc)) != EOF) {
					if (c == '\n') {
						this->log("MKFONT: " + row, Logger::INFO);
					} else {
						row += char(c);
					}
				}
				pclose(mkfont_proc);
			}
		}

		this->_reloadRequired = false;
		return true;
	}
	else
		return false;
}

bool SettingsManagerDataStore::setValue(std::string const& name, std::string const& value){
	for (std::list<SettingRow>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (name == iter->name){

			if (iter->value != value){ //only set when the new value is really new
				iter->value = value;
				if (name == "GRUB_DISABLE_LINUX_RECOVERY" || name == "GRUB_DISABLE_OS_PROBER")
					_reloadRequired = true;
			}

			return true;
		}
	}

	settings.push_back(SettingRow());
	settings.back().name = name;
	settings.back().value = value;
	settings.back().validate();
	if (name == "GRUB_DISABLE_LINUX_RECOVERY" || name == "GRUB_DISABLE_OS_PROBER")
		_reloadRequired = true;
	return false;
}

bool SettingsManagerDataStore::setIsActive(std::string const& name, bool value){
	for (std::list<SettingRow>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (name == iter->name){
			if (iter->isActive != value){
				iter->isActive = value;
				if (name == "GRUB_DISABLE_LINUX_RECOVERY" || name == "GRUB_DISABLE_OS_PROBER")
					_reloadRequired = true;
			}
			return true;
		}
	}
	return false;
}
