#ifndef MOUNT_TABLE_INCLUDED
#define MOUNT_TABLE_INCLUDED
#include <list>
#include <cstdio>
#include <string>
struct MTabEntry {
	std::string device, mountpoint, fileSystem, options, dump, pass;
	bool isValid() const;
	operator bool() const;
};

class MountTable : public std::list<MTabEntry> {
	bool loaded;
	public:
	MountTable(FILE* source);
	MountTable();
	bool isLoaded() const;
	operator bool() const;
	void loadData(FILE* source);
	MTabEntry getEntryByMountpoint(std::string const& mountPoint);
};
#endif

