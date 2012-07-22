#include "umount_all.h"

bool umount_all(std::string const& base_mountpoint){
	FILE* mtabFile = fopen((base_mountpoint+"/etc/mtab").c_str(), "r");
	if (mtabFile){
		MountTable mtab(mtabFile);
		fclose(mtabFile);
		for (MountTable::iterator iter = mtab.begin(); iter != mtab.end(); iter++){
			if (iter->mountpoint != "/"){
				std::cout << "umounting " << iter->mountpoint << std::endl;
				system(("chroot '"+base_mountpoint+"' umount '"+iter->mountpoint+"'").c_str());
			}
		}
	}
	
	//theese mountpoints may be not on the target system's mtab… so force its umount
	system(("umount '"+base_mountpoint+"/dev'").c_str());
	system(("umount '"+base_mountpoint+"/proc'").c_str());
	system(("umount '"+base_mountpoint+"/sys'").c_str());
	
	bool success = system(("umount "+base_mountpoint).c_str()) == 0;
	if (success)
		rmdir(base_mountpoint.c_str());
	return success;
}
