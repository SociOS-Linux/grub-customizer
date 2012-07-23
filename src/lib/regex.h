#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <glib.h>
#include <iostream>

class Regex {
public:
	enum Exception {
		REGEX_PATTERN_NOT_MATCHING
	};
	static std::vector<std::string> match(std::string const& pattern, std::string const& str);
	static std::string replace(std::string const& pattern, std::string const& str, std::map<int, std::string> const& newValues);
};

#endif
