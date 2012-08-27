#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <glib.h>
#include <iostream>
#include "Exception.h"

class Regex {
public:
	static std::vector<std::string> match(std::string const& pattern, std::string const& str);
	static std::string replace(std::string const& pattern, std::string const& str, std::map<int, std::string> const& newValues);
};

#endif
