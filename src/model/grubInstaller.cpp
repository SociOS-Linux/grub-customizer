#include "grubInstaller.h"

GrubInstaller::GrubInstaller(GrubEnv& env)
	: env(env)
{
}

void GrubInstaller::threadable_install(std::string device){
	this->install_result = install(device);
	if (eventListener)
		eventListener->grubInstallCompleted(this->install_result);
}

std::string GrubInstaller::install(std::string device){
	FILE* install_proc = popen((this->env.install_cmd+" '"+device+"' 2>&1").c_str(), "r");
	std::string output;
	int c;
	while ((c = fgetc(install_proc)) != EOF){
		output += c;
	}
	int success = pclose(install_proc);
	if (success == 0)
		return ""; //empty return string = no error
	else
		return output;
}

void GrubInstaller::setEventListener(EventListenerModel_iface& eventListener) {
	this->eventListener = &eventListener;
}
