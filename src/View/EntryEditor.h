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

#ifndef ENTRYEDITDLG_H_
#define ENTRYEDITDLG_H_
#include <string>
#include <map>
#include <list>

class EntryEditDlg {
public:
	virtual inline ~EntryEditDlg() {};

	virtual void show() = 0;
	virtual void setSourcecode(std::string const& source) = 0;
	virtual std::string getSourcecode() = 0;

	virtual void addOption(std::string const& name, std::string const& value) = 0;
	virtual void setOptions(std::map<std::string, std::string> options) = 0;
	virtual std::map<std::string, std::string> getOptions() const = 0;
	virtual void removeOptions() = 0;

	virtual void setRulePtr(void* rulePtr) = 0;
	virtual void* getRulePtr() = 0;

	virtual void hide() = 0;

	virtual void setAvailableEntryTypes(std::list<std::string> const& names) = 0;
	virtual void selectType(std::string const& name) = 0;
	virtual std::string getSelectedType() const = 0;
};

#endif /* ENTRYEDITDLG_H_ */
