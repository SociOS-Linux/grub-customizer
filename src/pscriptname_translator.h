#ifndef PSCRIPTNAME_TRANSLATOR_INCLUDED
#define PSCRIPTNAME_TRANSLATOR_INCLUDED

#include <iostream>
#include <string>
#include <sstream>

std::string pscriptname_decode(std::string const& input);
std::string pscriptname_encode(std::string const& input, int x);

#endif
