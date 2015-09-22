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

#ifndef ENTRY_EDIT_CONTROLLER_INCLUDED
#define ENTRY_EDIT_CONTROLLER_INCLUDED

#include "../View/Main.hpp"
#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.hpp"

#include "../Model/Env.hpp"

#include "../View/EntryEditor.hpp"
#include "../View/Trait/ViewAware.hpp"

#include "../Model/ListCfg.hpp"
#include "../lib/Trait/LoggerAware.hpp"
#include "Common/ControllerAbstract.hpp"
#include "../lib/ContentParserFactory.hpp"
#include "../lib/Exception.hpp"
#include "../Model/DeviceDataList.hpp"
#include "../Model/Installer.hpp"
#include "../Model/ListCfg.hpp"
#include "Helper/Thread.hpp"


class EntryEditController :
	public Controller_Common_ControllerAbstract,
	public View_Trait_ViewAware<View_EntryEditor>,
	public Model_ListCfg_Connection,
	public ContentParserFactory_Connection,
	public Model_DeviceDataListInterface_Connection,
	public Model_Env_Connection,
	public Controller_Helper_Thread_Connection,
	public Bootstrap_Application_Object_Connection
{
	private: std::shared_ptr<ContentParser> currentContentParser;

	public: EntryEditController() :
		Controller_Common_ControllerAbstract("entry-edit"),
		currentContentParser(NULL)
	{
	}

	public: void initViewEvents() override
	{
		using namespace std::placeholders;
		this->view->onApplyClick = std::bind(std::mem_fn(&EntryEditController::applyAction), this);
		this->view->onSourceModification = std::bind(std::mem_fn(&EntryEditController::syncOptionsAction), this);
		this->view->onOptionModification = std::bind(std::mem_fn(&EntryEditController::syncSourceAction), this);
		this->view->onTypeSwitch = std::bind(std::mem_fn(&EntryEditController::switchTypeAction), this, _1);
		this->view->onNameChange = std::bind(std::mem_fn(&EntryEditController::validateNameAction), this);
		this->view->onFileChooserSelection = std::bind(std::mem_fn(&EntryEditController::replaceByFullPathAction), this, _1, _2, _3);
	}

	public: void initApplicationEvents() override
	{
		using namespace std::placeholders;
		this->applicationObject->onEntryEditorShowRequest.addHandler(std::bind(std::mem_fn(&EntryEditController::showAction), this, _1));
	}

	public: void showAction(Rule* rule)
	{
		if (rule == nullptr) {
			this->showCreatorAction();
			return;
		}
		try {
			this->_initTypes();
			this->view->setRulePtr(rule);
			this->view->setName(Model_Rule::fromPtr(rule).outputName);
			this->view->setSourcecode(Model_Rule::fromPtr(rule).dataSource->content);
			if (Model_Rule::fromPtr(rule).dataSource->type == Model_Entry::PLAINTEXT) {
				this->view->selectType("[TEXT]");
				this->view->setNameFieldVisibility(false);
			} else {
				this->syncEntryEditDlg(false);
				this->view->setNameFieldVisibility(true);
			}
			this->view->setTypeIsValid(true);
			this->view->setApplyEnabled(true);
			this->view->show();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void showCreatorAction()
	{
		this->logActionBegin("show-creator");
		try {
			this->_initTypes();
			this->view->setRulePtr(NULL);
			this->view->setName("");
			this->view->setSourcecode("");
			this->view->selectType("[NONE]");
			this->view->setTypeIsValid(false);
			this->view->setOptions(std::map<std::string, std::string>());
			this->view->setNameFieldVisibility(true);
			this->view->setApplyEnabled(false);
			this->validateNameAction();
			this->view->show();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void syncOptionsAction()
	{
		this->logActionBegin("sync-options");
		try {
			if (this->view->getSelectedType() != "[TEXT]") {
				this->syncEntryEditDlg(false);
			}
			this->_validate();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void syncSourceAction()
	{
		this->logActionBegin("sync-source");
		try {
			this->syncEntryEditDlg(true);
			this->_validate();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void syncEntryEditDlg(bool useOptionsAsSource)
	{
		try {
			if (useOptionsAsSource) {
				this->_updateSource(this->view->getOptions());
			} else {
				this->currentContentParser = this->contentParserFactory->create(this->view->getSourcecode());
				this->view->setOptions(this->currentContentParser->getOptions());
			}

			this->view->selectType(this->contentParserFactory->getNameByInstance(*this->currentContentParser));

			this->_validate();
		} catch (ParserNotFoundException const& e) {
			this->view->selectType("");
			this->view->setOptions(std::map<std::string, std::string>());
		}
	}

	public: void switchTypeAction(std::string const& newType)
	{
		this->logActionBegin("switch-type");
		try {
			if (newType != "" && newType != "[TEXT]") {
				this->currentContentParser = this->contentParserFactory->createByName(newType);
				this->currentContentParser->buildDefaultEntry();
				try {
					this->view->setSourcecode(this->currentContentParser->buildSource());
					this->view->setApplyEnabled(true);
				} catch (ParserException const& e) {
					this->view->showSourceBuildError();
					this->view->setApplyEnabled(false);
				}

				this->view->setOptions(this->currentContentParser->getOptions());
			} else {
				this->view->setOptions(std::map<std::string, std::string>());
				this->view->setSourcecode("");
				this->view->setApplyEnabled(true);
			}
			this->view->selectType(newType);
			if (newType == "[TEXT]") {
				this->view->setNameFieldVisibility(false);
				this->view->setName("#text");
			} else {
				this->view->setNameFieldVisibility(true);
				if (this->view->getName() == "#text") {
					this->view->setName("");
				}
			}
			this->view->setNameFieldVisibility(newType != "[TEXT]");
			this->view->setTypeIsValid(true);
			this->_validate();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	
	public: void applyAction()
	{
		this->logActionBegin("apply");
		try {
			Model_Rule* rulePtr = NULL;
			if (this->view->getRulePtr() != NULL) {
				rulePtr = &Model_Rule::fromPtr(this->view->getRulePtr());
			}
			bool isAdded = false;

			Model_Entry::EntryType type = this->view->getSelectedType() == "[TEXT]" ? Model_Entry::PLAINTEXT : Model_Entry::MENUENTRY;
			Model_Rule::RuleType ruleType = type == Model_Entry::PLAINTEXT ? Model_Rule::PLAINTEXT : Model_Rule::NORMAL;

			if (rulePtr == NULL) { // insert
				Model_Script* script = this->grublistCfg->repository.getCustomScript();
				if (script == NULL) {
					script = this->createCustomScript();
				}
				assert(script != NULL);
				script->entries().push_back(Model_Entry("new", "", "", type));
	
				Model_Rule newRule(script->entries().back(), true, *script);
	
				std::list<Model_Proxy*> proxies = this->grublistCfg->proxies.getProxiesByScript(*script);
				if (proxies.size() == 0) {
					this->grublistCfg->proxies.push_back(std::make_shared<Model_Proxy>(*script, false));
					proxies = this->grublistCfg->proxies.getProxiesByScript(*script);
				}
				assert(proxies.size() != 0);
	
				for (std::list<Model_Proxy*>::iterator proxyIter = proxies.begin(); proxyIter != proxies.end(); proxyIter++) {
					(*proxyIter)->rules.push_back(newRule);
					newRule.isVisible = false; // if there are more rules of this type, add them invisible
				}
				rulePtr = &proxies.front()->rules.back();
				isAdded = true;
			} else { // update
				Model_Script* script = this->grublistCfg->repository.getScriptByEntry(*rulePtr->dataSource);
				assert(script != NULL);
	
				if (!script->isCustomScript) {
					script = this->grublistCfg->repository.getCustomScript();
					if (script == NULL) {
						script = this->createCustomScript();
					}
					assert(script != NULL);
					script->entries().push_back(*rulePtr->dataSource);
	
					Model_Rule ruleCopy = *rulePtr;
					rulePtr->setVisibility(false);
					ruleCopy.dataSource = &script->entries().back();
					Model_Proxy* proxy = this->grublistCfg->proxies.getProxyByRule(rulePtr);
					std::list<Model_Rule>& ruleList = proxy->getRuleList(proxy->getParentRule(rulePtr));
	
					Model_Rule dummySubmenu(Model_Rule::SUBMENU, std::list<std::string>(), "DUMMY", true);
					dummySubmenu.subRules.push_back(ruleCopy);
					std::list<Model_Rule>::iterator iter = ruleList.insert(proxy->getListIterator(*rulePtr, ruleList), dummySubmenu);
	
					Model_Rule& insertedRule = iter->subRules.back();
					rulePtr = &this->grublistCfg->moveRule(&insertedRule, -1);
					this->grublistCfg->renumerate();
	
					std::list<Model_Proxy*> proxies = this->grublistCfg->proxies.getProxiesByScript(*script);
					for (std::list<Model_Proxy*>::iterator proxyIter = proxies.begin(); proxyIter != proxies.end(); proxyIter++) {
						if (!(*proxyIter)->getRuleByEntry(*rulePtr->dataSource, (*proxyIter)->rules, rulePtr->type)) {
							(*proxyIter)->rules.push_back(Model_Rule(*rulePtr->dataSource, false, *script));
						}
					}
				}
			}
	
			std::string newCode = this->view->getSourcecode();
			rulePtr->dataSource->content = newCode;
			rulePtr->dataSource->isModified = true;
			rulePtr->dataSource->type = type;
			rulePtr->dataSource->name = this->view->getName();
			rulePtr->outputName = this->view->getName();
			rulePtr->type = ruleType;
	
			this->env->modificationsUnsaved = true;
			this->applicationObject->onListModelChange.exec();
			this->applicationObject->onListRuleChange.exec(rulePtr, false);
	
			this->currentContentParser = nullptr;
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void replaceByFullPathAction(std::string newProperty, std::string value, std::list<std::string> oldProperties)
	{
		this->logActionBegin("replace-by-full-path");
		try {
			std::map<std::string, std::string> options = this->view->getOptions();
			for (std::list<std::string>::iterator oldPropIter = oldProperties.begin(); oldPropIter != oldProperties.end(); oldPropIter++) {
				options.erase(*oldPropIter);
			}
			options[newProperty] = value;
			this->view->setOptions(options);
			this->_updateSource(options);
			this->_validate();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void validateNameAction()
	{
		this->logActionBegin("validate-name");
		try {
			this->view->setNameIsValid(this->view->getName() != "");
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void _initTypes()
	{
		this->view->setAvailableEntryTypes(this->contentParserFactory->getNames());
	}

	public: void _validate()
	{
		if (this->currentContentParser == NULL) {
			return;
		}

		this->view->setErrors(this->currentContentParser->getErrors());
	}

	public: void _updateSource(std::map<std::string, std::string> const& options)
	{
		assert(this->currentContentParser != NULL);
		this->currentContentParser->setOptions(options);
		try {
			this->view->setSourcecode(this->currentContentParser->buildSource());
			this->view->setApplyEnabled(true);
		} catch (RegExNotMatchedException const& e) {
			this->view->showSourceBuildError();
			this->view->setApplyEnabled(false);
		}
	}

	private: Model_Script* createCustomScript() {
		this->grublistCfg->repository.push_back(Model_Script("custom", ""));
		Model_Script& script = this->grublistCfg->repository.back();
		script.isCustomScript = true;
		return &script;
	}
};

#endif
