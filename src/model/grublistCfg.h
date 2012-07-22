#ifndef GRUB_CUSTOMIZER_GrublistCfg_INCLUDED
#define GRUB_CUSTOMIZER_GrublistCfg_INCLUDED
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
#include "../interface/grubconf_ui.h"
#include "../config.h"
#include "mountTable.h"
#include "umount_all.h"
#include "settings_mng_ds.h"
#include "grubEnv.h"
#include "../interface/eventListener_model_iface.h"



class GrublistCfg {
	GrubConfUI* connectedUI;
	EventListenerModel_iface* eventListener;
	
	double progress;
public:
	GrublistCfg(GrubEnv& env);
	void setEventListener(EventListenerModel_iface& eventListener);
	
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
	GrubEnv& env;
	bool umountSwitchedRootPartition();
	
	bool cancelThreadsRequested;
	bool createScriptForwarder(std::string const& scriptName) const;
	bool removeScriptForwarder(std::string const& scriptName) const;
	std::string readScriptForwarder(std::string const& scriptForwarderFilePath) const;
	void load(bool keepConfig = false);
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
	
	bool compare(GrublistCfg const& other) const;
	
	void renameRule(Rule* rule, std::string const& newName);
};

#endif
