#ifndef UMOUNT_ALL_INCLUDED
#define UMOUNT_ALL_INCLUDED
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "mountTable.h"
bool umount_all(std::string const& base_mountpoint);
#endif
