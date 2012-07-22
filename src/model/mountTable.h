#ifndef MOUNT_TABLE_INCLUDED
#define MOUNT_TABLE_INCLUDED
#include <list>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <iostream>

struct MountException {
	enum Type {
		MOUNT_FAILED,
		MOUNT_ERR_NO_FSTAB
	};
	Type type;
	MountException(Type type);
};

struct Mountpoint {
	std::string device, mountpoint, fileSystem, options, dump, pass;
	bool isMounted;
	bool isValid(std::string const& prefix = "") const;
	operator bool() const;
	Mountpoint(std::string const& mountpoint = "", bool isMounted = false);
	Mountpoint(std::string const& device, std::string const& mountpoint, std::string const& options, bool isMounted = false);
	void mount();
	void umount();
};

//TODO: Problem: Mountpoints werden mal mit relativen, mal mit absoluten Pfaden erstellt! VEREINHEITLICHEN!
class MountTable : public std::list<Mountpoint> {
	bool loaded;
	public:
	MountTable(FILE* source, std::string const& rootDirectory, bool default_isMounted_flag = false);
	MountTable(std::string const& rootDirectory);
	MountTable();
	void sync_isMountedStats(MountTable const& mtab);
	bool isLoaded() const;
	operator bool() const;
	void loadData(FILE* source, std::string const& prefix, bool default_isMounted_flag = false);
	void loadData(std::string const& rootDirectory);
	void clear(std::string const& prefix);
	Mountpoint getEntryByMountpoint(std::string const& mountPoint) const;
	Mountpoint& getEntryRefByMountpoint(std::string const& mountPoint);
	Mountpoint& add(Mountpoint const& mpToAdd);
	void remove(Mountpoint const& mountpoint);
	void umountAll(std::string const& prefix);
	void mountRootFs(std::string const& device, std::string const& mountpoint);
	void print();
};
#endif

