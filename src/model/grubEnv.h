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
#include <iostream>
#include "mountTable.h"
#include <cstdlib>
#include <dirent.h>

struct GrubEnv {
	enum Mode {
		GRUB_MODE,
		BURG_MODE
	};
	GrubEnv();
	bool init(GrubEnv::Mode mode, std::string const& dir_prefix);
	static bool check_cmd(std::string const& cmd, std::string const& cmd_prefix = "");
	static bool check_dir(std::string const& dir);
	std::string getRootDevice();
	std::string cfg_dir, cfg_dir_noprefix, mkconfig_cmd, cfg_dir_prefix, update_cmd, install_cmd, output_config_file, output_config_dir, settings_file;
	bool burgMode;
	std::list<GrubEnv::Mode> getAvailableModes();
};

#endif
