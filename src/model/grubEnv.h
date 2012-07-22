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
