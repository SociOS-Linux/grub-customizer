#ifndef FB_RESOLUTIONS_GETTER
#define FB_RESOLUTIONS_GETTER
#include <string>
#include <list>
#include <iostream>
#include <cstdio>
#include "settings_dlg.h"

class FbResolutionsGetter {
	std::list<std::string> data;
	GrubSettingsDlg* outputUI;
public:
	FbResolutionsGetter();
	const std::list<std::string>& getData() const;
	void load();
	void connectUI(GrubSettingsDlg& ui);
};

#endif
