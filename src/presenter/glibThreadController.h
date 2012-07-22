#ifndef GLIBTHREADCONTROLLER_H_INCLUDED
#define GLIBTHREADCONTROLLER_H_INCLUDED
#include "../interface/threadController.h"
#include <glibmm/thread.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/main.h>
#include "grubCustomizer.h"

class GlibThreadController : public ThreadController {
	GrubCustomizer& app;

	Glib::Dispatcher disp_sync_load, disp_sync_save, disp_thread_died, disp_updateSettingsDlgResolutionList, disp_settings_loaded;
public:
	GlibThreadController(GrubCustomizer& app);
	void syncEntryList();
	void updateSaveProgress();
	void updateSettingsDlgResolutionList();
	void showThreadDiedError();
	void enableSettings();
	void startLoadThread(bool preserveConfig);
	void startSaveThread();
	void startFramebufferResolutionLoader();
	void startGrubInstallThread(std::string const& device);
	void stopApplication();
};

#endif
