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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
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

#include "Proxylist.h"
#include "Repository.h"
#include "ProxyScriptData.h"
#include "../config.h"
#include "MountTable.h"
#include "SettingsManagerData.h"
#include "Env.h"
#include "../Controller/MainController.h"

#include "../lib/assert.h"

#include "../lib/Mutex.h"
#include "../lib/CommonClass.h"

#include "../lib/Exception.h"
#include "../lib/ArrayStructure.h"
#include "../lib/trim.h"
#include "ScriptSourceMap.h"
#include <stack>


class Model_ListCfg : public CommonClass {
	MainController* eventListener;
	
	double progress;
	std::string progress_name;
	int progress_pos, progress_max;
	Mutex* mutex;
	std::string errorLogFile;

	Model_ScriptSourceMap scriptSourceMap;
public:
	Model_ListCfg(Model_Env& env);
	void setEventListener(MainController& eventListener);
	void setMutex(Mutex& mutex);
	void setLogger(Logger& logger);

	Model_Proxylist proxies;
	Model_Repository repository;
	
	bool verbose;
	bool error_proxy_not_found;
	Model_Env& env;
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
	std::map<Model_Entry const*, Model_Script const*> getEntrySources(Model_Proxy const& proxy, Model_Rule const* parent = NULL) const;
	bool loadStaticCfg();

	void send_new_load_progress(double newProgress, std::string scriptName = "", int current = 0, int max = 0);
	void send_new_save_progress(double newProgress);
	void cancelThreads();
	void reset();
	double getProgress() const;
	std::string getProgress_name() const;
	int getProgress_pos() const;
	int getProgress_max() const;

	void increaseProxyPos(Model_Proxy* proxy);
	void renumerate(bool favorDefaultOrder = true);
	
	void swapRules(Model_Rule* a, Model_Rule* b);
	Model_Rule& moveRule(Model_Rule* rule, int direction);
	void swapProxies(Model_Proxy* a, Model_Proxy* b);
	
	Model_Rule* createSubmenu(Model_Rule* position);
	Model_Rule* splitSubmenu(Model_Rule* child);

	bool cfgDirIsClean();
	void cleanupCfgDir();
	
	bool compare(Model_ListCfg const& other) const;
	static std::list<Model_Rule const*> getComparableRules(std::list<Model_Rule> const& list);
	static bool compareLists(std::list<Model_Rule const*> a, std::list<Model_Rule const*> b);

	void renameRule(Model_Rule* rule, std::string const& newName);
	std::string getRulePath(Model_Rule& rule);
	std::string getGrubErrorMessage() const;

	void addColorHelper();

	std::list<Model_Entry*> getRemovedEntries(Model_Entry* parent = NULL, bool ignorePlaceholders = false);
	Model_Rule* addEntry(Model_Entry& entry);

	void deleteEntry(Model_Entry const& entry);

	std::list<Rule*> getNormalizedRuleOrder(std::list<Rule*> rules);

	std::list<Model_Script*> getProxifiedScripts();
	void generateScriptSourceMap();
	void populateScriptSourceMap();
	bool hasScriptUpdates() const;
	void applyScriptUpdates();

	operator ArrayStructure() const;
};

#endif
