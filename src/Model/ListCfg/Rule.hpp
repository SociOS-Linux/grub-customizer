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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef GRUB_CUSTOMIZER_RULE_INCLUDED
#define GRUB_CUSTOMIZER_RULE_INCLUDED
#include <string>
#include <ostream>
#include <memory>

namespace Gc { namespace Model { namespace ListCfg { class Rule
	: public Gc::Common::Type::Rule
{
	public: std::shared_ptr<Gc::Model::ListCfg::Entry> dataSource; //assigned when using RuleType::OTHER_ENTRIES_PLACEHOLDER
	public: std::string outputName;
	public: std::string __idHash; //should only be used by sync()!
	public: std::list<std::string> __idpath; //should only be used by sync()!
	public: std::string __sourceScriptPath; //should only be used by sync()!
	public: bool isVisible;
	public: std::list<std::shared_ptr<Gc::Model::ListCfg::Rule>> subRules;
	public: enum RuleType {
		NORMAL, OTHER_ENTRIES_PLACEHOLDER, PLAINTEXT, SUBMENU
	};

	public: RuleType type;

	public: Rule(RuleType type, std::list<std::string> path, std::string outputName, bool isVisible)
		: type(type), isVisible(isVisible), __idpath(path), outputName(outputName), dataSource(nullptr)
	{}

	public: Rule(RuleType type, std::list<std::string> path, bool isVisible)
		: type(type), isVisible(isVisible), __idpath(path), outputName(path.back()), dataSource(nullptr)
	{}

	//generate rule for given entry
	public: Rule(
		std::shared_ptr<Gc::Model::ListCfg::Entry> source,
		bool isVisible,
		std::shared_ptr<Gc::Model::ListCfg::EntryPathFollower> pathFollower,
		std::list<std::list<std::string>> const& pathesToIgnore = std::list<std::list<std::string>>(),
		std::list<std::string> const& currentPath = std::list<std::string>()
	) :
		type(source->type == Gc::Model::ListCfg::Entry::PLAINTEXT ? Gc::Model::ListCfg::Rule::PLAINTEXT : (source->type == Gc::Model::ListCfg::Entry::SUBMENU ? Gc::Model::ListCfg::Rule::SUBMENU : Gc::Model::ListCfg::Rule::NORMAL)),
		isVisible(isVisible),
		__idpath(currentPath),
		outputName(source->name),
		dataSource(source->type == Gc::Model::ListCfg::Entry::SUBMENU ? nullptr : source)
	{
		if (source->type == Gc::Model::ListCfg::Entry::SUBMENU) {
			auto placeholder = std::make_shared<Gc::Model::ListCfg::Rule>(
				Gc::Model::ListCfg::Rule::OTHER_ENTRIES_PLACEHOLDER,
				currentPath,
				"*",
				this->isVisible
			);
			placeholder->dataSource = pathFollower->getEntryByPath(currentPath);
			this->subRules.push_front(placeholder);
		}
		for (auto entry : source->subEntries) {
			std::list<std::string> currentPath_in_loop = currentPath;
			currentPath_in_loop.push_back(entry->name);

			//find out if currentPath is on the blacklist
			bool currentPath_in_loop_is_blacklisted = false;
			for (auto pathToIgnore : pathesToIgnore) {
				if (pathToIgnore == currentPath_in_loop) {
					currentPath_in_loop_is_blacklisted = true;
					break;
				}
			}

			//add this entry as rule if not blacklisted
			if (!currentPath_in_loop_is_blacklisted){
				this->subRules.push_back(
					std::make_shared<Gc::Model::ListCfg::Rule>(
						entry,
						isVisible,
						pathFollower,
						pathesToIgnore,
						currentPath_in_loop
					)
				);
			}
		}
	}

	public: Rule()
		: type(Gc::Model::ListCfg::Rule::NORMAL), isVisible(false), dataSource(nullptr)
	{}

	public: std::string toString(Gc::Model::ListCfg::EntryPathBuilder const& pathBuilder) {
		std::string result = isVisible ? "+" : "-";
		if (type == Gc::Model::ListCfg::Rule::PLAINTEXT) {
			result += "#text";
		} else if (dataSource) {
			result += pathBuilder.buildPathString(this->dataSource, this->type == OTHER_ENTRIES_PLACEHOLDER);
			if (this->dataSource->content.size() && this->type != Gc::Model::ListCfg::Rule::OTHER_ENTRIES_PLACEHOLDER) {
				result += "~" + Gc::Common::Functions::md5(this->dataSource->content) + "~";
			}
		} else if (type == Gc::Model::ListCfg::Rule::SUBMENU) {
			result += "'SUBMENU'"; // dummy data source
		} else {
			result += "???";
		}
		if (type == Gc::Model::ListCfg::Rule::SUBMENU || (type == Gc::Model::ListCfg::Rule::NORMAL && dataSource && dataSource->name != outputName)) {
			result += " as '"+Gc::Common::Functions::str_replace("'", "''", outputName)+"'";
		}
	
		if (this->dataSource) {
			std::string sourceScriptPath = pathBuilder.buildScriptPath(this->dataSource);
			if (sourceScriptPath != "") {
				result += " from '" + sourceScriptPath + "'";
			}
		}
	
		if (type == Gc::Model::ListCfg::Rule::SUBMENU && this->subRules.size() > 0) {
			result += "{";
			for (auto iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
				if (iter != this->subRules.begin())
					result += ", ";
				result += iter->get()->toString(pathBuilder);
			}
			result += "}";
		}
		return result;
	}

	public: bool hasRealSubrules() const {
		for (auto subRule : this->subRules) {
			if (subRule->isVisible && ((subRule->type == Gc::Model::ListCfg::Rule::NORMAL && subRule->dataSource) || (subRule->type == Gc::Model::ListCfg::Rule::SUBMENU && subRule->hasRealSubrules()))) {
				return true;
			}
		}
		return false;
	}

	public: void print(std::ostream& out) const {
		if (this->isVisible) {
			if (this->type == Gc::Model::ListCfg::Rule::PLAINTEXT && this->dataSource) {
				out << this->dataSource->content;
			} else if (this->type == Gc::Model::ListCfg::Rule::NORMAL && this->dataSource) {
				out << "menuentry";
				out << " \"" << this->outputName << "\"" << this->dataSource->extension << "{\n";
				out << this->dataSource->content;
				out << "}\n";
			} else if (this->type == Gc::Model::ListCfg::Rule::SUBMENU && this->hasRealSubrules()) {
				out << "submenu" << " \"" << this->outputName << "\"" << "{\n";
				for (auto rule : this->subRules) {
					rule->print(out);
				}
				out << "}\n";
			}
		}
	}

	public: std::string getEntryName() const {
		if (this->dataSource)
			return this->dataSource->name;
		else
			return "?";
	}

	public: void setVisibility(bool isVisible) {
		this->isVisible = isVisible;
		for (auto rule : this->subRules) {
			rule->setVisibility(isVisible);
		}
	}

	public: std::shared_ptr<Gc::Model::ListCfg::Rule> clone()
	{
		auto result = std::make_shared<Gc::Model::ListCfg::Rule>(*this);

		result->subRules.clear();

		for (auto subRule : this->subRules) {
			result->subRules.push_back(subRule->clone());
		}

		return result;
	}

	public: operator Gc::Common::ArrayStructure::Container() const {
		Gc::Common::ArrayStructure::Container result;

		result["dataSource"] = this->dataSource.get();
		result["outputName"] = this->outputName;
		result["__idHash"] = this->__idHash;
		result["__idpath"] = Gc::Common::ArrayStructure::Container(this->__idpath);
		result["__sourceScriptPath"] = this->__sourceScriptPath;
		result["isVisible"] = this->isVisible;
		result["subRules"].isArray = true;
		int i = 0;
		for (auto rule : this->subRules) {
			result["subRules"][i] = Gc::Common::ArrayStructure::Container(*rule);
			i++;
		}
		result["type"] = this->type;

		return result;
	}
};}}}

#endif
