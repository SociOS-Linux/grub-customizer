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

#ifndef ENTRY_EDIT_CONTROLLER_INCLUDED
#define ENTRY_EDIT_CONTROLLER_INCLUDED

#include "../Model/ListCfg.h"
#include "../View/Main.h"
#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.h"

#include "../Model/Env.h"

#include "../Model/Installer.h"
#include "../View/EntryEditor.h"

#include "../Model/ListCfg.h"
#include "../Model/DeviceDataList.h"
#include "../interface/contentParserFactory.h"

#include "../lib/CommonClass.h"
#include "../Controller/ControllerAbstract.h"


class EntryEditControllerImpl : public EntryEditController, public ControllerAbstract {
	Model_Env& env;
	Model_ListCfg* grublistCfg;
	View_EntryEditor* view;
	ContentParserFactory* contentParserFactory;
	ContentParser* currentContentParser;
	DeviceDataList_Iface* deviceDataList;
public:
	EntryEditControllerImpl(Model_Env& env);

	void setDeviceDataList(Model_DeviceDataList& deviceDataList);
	void setContentParserFactory(ContentParserFactory& contentParserFactory);
	void setListCfg(Model_ListCfg& grublistCfg);
	void setView(View_EntryEditor& view);

	void showAction(void* rule);
	void showCreatorAction();
	void syncOptionsAction();
	void syncSourceAction();
	void syncEntryEditDlg(bool useOptionsAsSource);
	void switchTypeAction(std::string const& newType);
	
	void applyAction();
};

#endif
