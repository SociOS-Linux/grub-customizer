/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GLIBTHREADCONTROLLER_H_INCLUDED
#define GLIBTHREADCONTROLLER_H_INCLUDED
#include "../interface/threadController.h"
#include <glibmm/thread.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/main.h>
#include "grubCustomizer.h"
#include "../ControllerCollection.h"
#include "../presenter/commonClass.h"

class GlibThreadController : public ThreadController, public CommonClass {
	GrubCustomizer& app;
	ControllerCollection& _controllers;

	Glib::Dispatcher disp_sync_load, disp_sync_save, disp_thread_died, disp_updateSettingsDlgResolutionList, disp_settings_loaded;
public:
	GlibThreadController(GrubCustomizer& app, ControllerCollection& controllers);
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
private:
	void _execLoadSync();
	void _execSaveSync();
	void _execLoad(bool preserveConfig);
	void _execSave();
	void _execDie();
	void _execActivateSettings();
};

#endif
