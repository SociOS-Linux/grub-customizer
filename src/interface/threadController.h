#ifndef THREADCONTROLLER_H_INCLUDED
#define THREADCONTROLLER_H_INCLUDED
#include <string>

class ThreadController {
public:
	virtual void syncEntryList() = 0;
	virtual void updateSaveProgress() = 0;
	virtual void updateSettingsDlgResolutionList() = 0;
	virtual void showThreadDiedError() = 0;
	virtual void enableSettings() = 0;
	virtual void startLoadThread(bool preserveConfig) = 0;
	virtual void startSaveThread() = 0;
	virtual void startFramebufferResolutionLoader() = 0;
	virtual void startGrubInstallThread(std::string const& device) = 0;
	virtual void stopApplication() = 0;
};

#endif
