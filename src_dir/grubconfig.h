#ifndef GRUBCONFIG_H_INCLUDED
#define GRUBCONFIG_H_INCLUDED
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <list>
#include <map>
#include <unistd.h> //zum Erstellen der Symlinks
#include <sstream>
#include <libintl.h>
#include <cerrno>
#include "confparser.h"
#include "libproxy.h"
#include "libproxyscript_parser.h"
#include "grubconf_ui.h"
#include "config.h"

std::string getProxifiedScriptName(std::string const& proxyScriptPath); //return value: name of proxy or emtpy string if it isn't a proxy


struct ToplevelScript {
	ToplevelScript(std::string name, std::string proxyfiedScriptName, int permissions);
	ToplevelScript();
	std::string name, proxyfiedScriptName;
	int permissions;
	bool isExecutable() const;
	void set_executable(bool is_executable);
	EntryList entries;
	
	std::string getBasename();
	short int index;
	bool isProxy;
};

bool compare_scripts(ToplevelScript const& a, ToplevelScript const& b);

class GrubConfig : public std::list<ToplevelScript> {
	GrubConfUI* connectedUI;
	double progress;
	std::string message;
	bool cancelThreadsRequested;
	FILE* mkconfigProc;
public:
	void cancelThreads();
	std::map<std::string, EntryList> realScripts;
	void send_new_load_progress(double newProgress);
	void send_new_save_progress(double newProgress);
	GrubConfig();
	void load();
	void save();
	void reset();
	void connectUI(GrubConfUI& ui);
	void increaseScriptPos(ToplevelScript* script);
	void renumerate();
	void copyScriptFromRepository(std::string const& name);
	GrubConfig::iterator getIterByPointer(ToplevelScript* ptr);
	
	double getProgress() const;
	std::string getMessage() const;
	
	void generateProxy(FILE* proxyFile, ToplevelScript* script);
};

#endif
