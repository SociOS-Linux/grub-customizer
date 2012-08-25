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

#ifndef GRUBCUSTOMIZERIFACE_H_
#define GRUBCUSTOMIZERIFACE_H_

#include <string>
#include <map>
#include <list>

class GrubCustomizerIface {
public:
	virtual void updateSettingsDlg() = 0;
	virtual void syncSettings() = 0;
	virtual void grubEnvSetRootDeviceName(std::string const& rootDevice) = 0;
	virtual void grubEnvsetEnvSettings(std::map<std::string, std::string> const& props, std::list<std::string> const& requiredProps, std::list<std::string> const& validProps) = 0;
	virtual void grubEnvShow(bool resetPartitionChooser = false) = 0;
	virtual void showAboutDialog() = 0;
	virtual void showInstallDialog() = 0;
	virtual void showMessageGrubInstallCompleted(std::string const& msg) = 0;
	virtual void showSettingsDlg() = 0;
	virtual void showEntryAddDlg() = 0;
	virtual void showEnvEditor(bool resetPartitionChooser = false) = 0;
};


#endif /* GRUBCUSTOMIZERIFACE_H_ */
