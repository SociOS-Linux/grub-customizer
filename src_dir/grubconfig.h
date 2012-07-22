#ifndef GRUB_CUSTOMIZER_GRUBCONFIG_INCLUDED
#define GRUB_CUSTOMIZER_GRUBCONFIG_INCLUDED
#include <list>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <sstream>
#include <map>
#include <libintl.h>
#include <unistd.h>

#include "proxylist.h"
#include "repository.h"
#include "libproxyscript_parser.h"
#include "grubconf_ui.h"
#include "config.h"
#include "mountTable.h"
#include "umount_all.h"

struct GrubConfEnvironment {
	enum Mode {
		GRUB_MODE,
		BURG_MODE
	};
	GrubConfEnvironment();
	bool init(GrubConfEnvironment::Mode mode, std::string const& dir_prefix);
	static bool check_cmd(std::string const& cmd, std::string const& cmd_prefix = "");
	static bool check_dir(std::string const& dir);
	static bool isLiveCD();
	std::string getRootDevice();
	std::string cfg_dir, cfg_dir_noprefix, mkconfig_cmd, cfg_dir_prefix, update_cmd, install_cmd, output_config_file, output_config_dir;
	bool burgMode;
};

class GrubConfig {
	GrubConfUI* connectedUI;
	
	double progress;
public:
	GrubConfig();
	
	//these locks prevents segfaults when loading… not perfect but much better than without
	bool read_lock; //prevent UIs of reading data
	bool write_lock; //prevent this config object of writing data (it waits until write_lock is false)
	ProxyList proxies;
	Repository repository;
	std::string install_result;
	std::string message;
	bool config_has_been_different_on_startup_but_unsaved;
	bool verbose;
	bool error_proxy_not_found;
	GrubConfEnvironment env;
	bool prepare(bool forceRootSelection = false);
	bool umountSwitchedRootPartition();
	
	bool cancelThreadsRequested;
	bool createScriptForwarder(std::string const& scriptName) const;
	bool removeScriptForwarder(std::string const& scriptName) const;
	std::string readScriptForwarder(std::string const& scriptForwarderFilePath) const;
	void load();
	void save();
	void readGeneratedFile(FILE* source, bool createScriptIfNotFound = false);

	void connectUI(GrubConfUI& ui);
	void send_new_load_progress(double newProgress);
	void send_new_save_progress(double newProgress);
	void cancelThreads();
	void threadable_install(std::string device);
	std::string install(std::string device);
	std::string getMessage() const;
	void reset();
	double getProgress() const;
	void increaseProxyPos(Proxy* proxy);
	void renumerate();
	
	bool cfgDirIsClean();
	void cleanupCfgDir();
	
	bool compare(GrubConfig const& other) const;
	
};

#endif
