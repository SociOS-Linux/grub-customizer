#include "grubInstaller.h"

GrubInstaller::GrubInstaller(GrubEnv& env)
	: env(env), eventListener(NULL)
{
}

void GrubInstaller::threadable_install(std::string const& device){
	this->install_result = install(device);
	if (eventListener)
		eventListener->grubInstallCompleted(this->install_result);
}

std::string GrubInstaller::install(std::string const& device){
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
