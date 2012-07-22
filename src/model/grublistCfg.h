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
#include "../config.h"
#include "mountTable.h"
#include "settings_mng_ds.h"
#include "grubEnv.h"
#include "../interface/evt_model.h"



class GrublistCfg {
	EventListener_model* eventListener;
	
	double progress;
	bool locked;
public:
	GrublistCfg(GrubEnv& env);
	void setEventListener(EventListener_model& eventListener);
	
	enum Exception {
		GRUB_CFG_DIR_NOT_FOUND,
		GRUB_CMD_EXEC_FAILED
	};

	ProxyList proxies;
	Repository repository;
	
	bool verbose;
	bool error_proxy_not_found;
	GrubEnv& env;
	void lock();
	bool lock_if_free();
	void unlock();
	
	bool cancelThreadsRequested;
	bool createScriptForwarder(std::string const& scriptName) const;
	bool removeScriptForwarder(std::string const& scriptName) const;
	std::string readScriptForwarder(std::string const& scriptForwarderFilePath) const;
	void load(bool preserveConfig = false);
	void save();
	void readGeneratedFile(FILE* source, bool createScriptIfNotFound = false);
	bool loadStaticCfg();

	void send_new_load_progress(double newProgress);
	void send_new_save_progress(double newProgress);
	void cancelThreads();
	void reset();
	double getProgress() const;
	void increaseProxyPos(Proxy* proxy);
	void renumerate();
	
	void swapRules(Rule* a, Rule* b);
	void swapProxies(Proxy* a, Proxy* b);
	
	bool cfgDirIsClean();
	void cleanupCfgDir();
	
	bool compare(GrublistCfg const& other) const;
	
	void renameRule(Rule* rule, std::string const& newName);
};

#endif
