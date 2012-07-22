#ifndef GRUB_INSTALLER_INCLUDED
#define GRUB_INSTALLER_INCLUDED
#include <string>
#include "grubEnv.h"
#include "../interface/eventListener_model_iface.h"

class GrubInstaller {
	GrubEnv& env;
	std::string install_result;
	EventListenerModel_iface* eventListener;
public:
	GrubInstaller(GrubEnv& env);
	void threadable_install(std::string const& device);
	std::string install(std::string const& device);
	void setEventListener(EventListenerModel_iface& eventListener);
};

#endif
