#ifndef GRUB_CUSTOMIZER_REPOSITORY_INCLUDED
#define GRUB_CUSTOMIZER_REPOSITORY_INCLUDED
#include "script.h"
#include "libproxyscript_parser.h"
#include "pscriptname_translator.h"
#include <sys/stat.h>
#include <dirent.h>

struct Repository : public std::list<Script> {
	void load(std::string const& directory, bool is_proxifiedScript_dir);
	Script* getScriptByFilename(std::string const& fileName, bool createScriptIfNotFound = false);
	Script* getNthScript(int pos);
	void deleteAllEntries();
};

#endif
