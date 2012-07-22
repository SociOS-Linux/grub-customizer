#ifndef GRUB_CUSTOMIZER_SCRIPT_INCLUDED
#define GRUB_CUSTOMIZER_SCRIPT_INCLUDED
#include <string>
#include <list>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include "entry.h"

struct Script : public std::list<Entry> {
	std::string name, fileName;
	Script(std::string const& name, std::string const& fileName);
	bool isInScriptDir(std::string const& cfg_dir) const;
	Entry* getEntryByName(std::string const& name);
	void moveToBasedir(std::string const& cfg_dir); //moves the file from any location to grub.d and adds the prefix PS_ (proxified Script) or DS_ (default script)
	bool moveFile(std::string const& newPath, short int permissions = -1);
};

#endif
