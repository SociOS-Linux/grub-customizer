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

#ifndef GRUBENV_INCLUDED
#define GRUBENV_INCLUDED

#include <string>
#include <cstdio>
#include "MountTable.h"
#include <cstdlib>
#include <dirent.h>
#include <map>
#include <sys/stat.h>
#include "../presenter/commonClass.h"
#include "SettingsStore.h"

struct Model_Env : public CommonClass {
public:
	enum Mode {
		GRUB_MODE,
		BURG_MODE
	};
	enum Exception {
		FILE_SAVE_FAILED
	};
	Model_Env();
	bool init(Model_Env::Mode mode, std::string const& dir_prefix);
	void loadFromFile(FILE* cfg_file, std::string const& dir_prefix);
	void save();
	std::map<std::string, std::string> getProperties();
	void setProperties(std::map<std::string, std::string> const& props);
	std::list<std::string> getRequiredProperties();
	std::list<std::string> getValidProperties();
	bool check_cmd(std::string const& cmd, std::string const& cmd_prefix = "") const;
	bool check_dir(std::string const& dir) const;
	bool check_file(std::string const& file) const;
	std::string trim_cmd(std::string const& cmd) const;
	std::string getRootDevice();
	std::string cfg_dir, cfg_dir_noprefix, mkconfig_cmd, mkfont_cmd, cfg_dir_prefix, update_cmd, install_cmd, output_config_file, output_config_dir, settings_file, devicemap_file, mkdevicemap_cmd, cmd_prefix;
	bool burgMode;
	bool useDirectBackgroundProps; // Whether background settings should be set directly or by creating a desktop-base script
	std::list<Model_Env::Mode> getAvailableModes();
};

#endif
