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

#ifndef GRUB_CUSTOMIZER_GrublistCfg_INCLUDED
#define GRUB_CUSTOMIZER_GrublistCfg_INCLUDED
#include <list>
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <map>
#include <libintl.h>
#include <unistd.h>
#include <fstream>

#include "proxylist.h"
#include "repository.h"
#include "libproxyscript_parser.h"
#include "../config.h"
#include "mountTable.h"
#include "settings_mng_ds.h"
#include "grubEnv.h"
#include "../interface/evt_model.h"

#include "../interface/mutex.h"

#include "../presenter/commonClass.h"


class GrublistCfg : public CommonClass {
	EventListener_model* eventListener;
	
	double progress;
	Mutex* mutex;
	std::string errorLogFile;
public:
	GrublistCfg(GrubEnv& env);
	void setEventListener(EventListener_model& eventListener);
	void setMutex(Mutex& mutex);
	void setLogger(Logger& logger);
	
	enum Exception {
		GRUB_CFG_DIR_NOT_FOUND,
		GRUB_CMD_EXEC_FAILED,
		NO_MOVE_TARGET_FOUND,
		MISSING_MUTEX
	};

	ProxyList proxies;
	Repository repository;
	
	bool verbose;
	bool error_proxy_not_found;
	GrubEnv& env;
	void lock();
	bool lock_if_free();
	void unlock();
	bool ignoreLock;
	
	bool cancelThreadsRequested;
	bool createScriptForwarder(std::string const& scriptName) const;
	bool removeScriptForwarder(std::string const& scriptName) const;
	std::string readScriptForwarder(std::string const& scriptForwarderFilePath) const;
	void load(bool preserveConfig = false);
	void save();
	void readGeneratedFile(FILE* source, bool createScriptIfNotFound = false, bool createProxyIfNotFound = false);
	std::map<Entry const*, Script const*> getEntrySources(Proxy const& proxy, Rule const* parent = NULL) const;
	bool loadStaticCfg();

	void send_new_load_progress(double newProgress);
	void send_new_save_progress(double newProgress);
	void cancelThreads();
	void reset();
	double getProgress() const;
	void increaseProxyPos(Proxy* proxy);
	void renumerate();
	
	void swapRules(Rule* a, Rule* b);
	Rule& moveRule(Rule* rule, int direction);
	void swapProxies(Proxy* a, Proxy* b);
	
	Rule* createSubmenu(Rule* position);
	Rule* splitSubmenu(Rule* child);

	bool cfgDirIsClean();
	void cleanupCfgDir();
	
	bool compare(GrublistCfg const& other) const;
	static std::list<Rule const*> getComparableRules(std::list<Rule> const& list);
	static bool compareLists(std::list<Rule const*> a, std::list<Rule const*> b);
	
	void renameRule(Rule* rule, std::string const& newName);
	std::string getGrubErrorMessage() const;

	void addColorHelper();

	std::list<Entry*> getRemovedEntries(Entry* parent = NULL);
	Rule* addEntry(Entry& entry);
};

#endif
