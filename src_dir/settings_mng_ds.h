#ifndef SETTING_MANAGER_DATASTORE_INCLUDED
#define SETTING_MANAGER_DATASTORE_INCLUDED
#include <list>
#include <string>
#include <cstdio>
#include <iostream>
#include <sys/stat.h> //mkdir

struct SettingRow {
	std::string name, value, comment, plaintext;
	bool hasExportPrefix, isActive, isSetting;
	void validate();
	std::string getOutput();
};

class SettingsManagerDataStore {
	std::list<SettingRow> settings;
	std::string filePath;
	bool isModified;
	bool _reloadRequired;
public:
	SettingsManagerDataStore();
	bool getIsModified() const;
	bool reloadRequired() const;
	std::list<SettingRow>::iterator begin(bool jumpOverPlaintext = true);
	std::list<SettingRow>::iterator end();
	void iter_to_next_setting(std::list<SettingRow>::iterator& iter);
	std::string getValue(std::string const& name);
	bool setValue(std::string const& name, std::string const& value);
	bool isActive(std::string const& name, bool checkValueToo = false);
	bool setIsActive(std::string const& name, bool value);
	bool setIsExport(std::string const& name, bool isExport);
	bool load(std::string const& filePath);
	bool save(std::string const& dir_prefix);
	void clear();
};
#endif
