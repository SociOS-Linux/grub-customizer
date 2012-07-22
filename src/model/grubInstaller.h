#ifndef GRUB_INSTALLER_INCLUDED
#define GRUB_INSTALLER_INCLUDED
#include <string>
#include "grubEnv.h"
#include "../interface/evt_model.h"

class GrubInstaller {
	GrubEnv& env;
	std::string install_result;
	EventListener_model* eventListener;
public:
	GrubInstaller(GrubEnv& env);
	void threadable_install(std::string const& device);
	std::string install(std::string const& device);
	void setEventListener(EventListener_model& eventListener);
};

#endif
