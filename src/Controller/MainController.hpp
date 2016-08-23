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

#ifndef MAINCONNTROLLER_INCLUDED
#define MAINCONNTROLLER_INCLUDED

#include "../Model/ListCfg/ListCfg.hpp"
#include "../View/Main.hpp"
#include "../View/Trait/ViewAware.hpp"
#include <libintl.h>
#include <locale.h>
#include <sstream>
#include <algorithm>
#include <functional>
#include <memory>
#include "../config.hpp"

#include "../Model/Env.hpp"

#include "../Model/ListCfg/ListCfg.hpp"
#include "../Model/Device/DeviceDataList.hpp"
#include "../Model/ContentParser/GenericFactory.hpp"

#include "Common/ControllerAbstract.hpp"

#include "../Model/Logger/Trait/LoggerAware.hpp"
#include "../Common/Exception.hpp"
#include "../View/Mapper/EntryName.hpp"
#include "../Model/FbResolutionsGetter.hpp"
#include "../View/Model/ListItem.hpp"
#include "Helper/DeviceInfo.hpp"
#include "Helper/Thread.hpp"

/**
 * This controller operates on the entry list
 */

namespace Gc { namespace Controller { class MainController :
	public Gc::Controller::Common::ControllerAbstract,
	public View_Trait_ViewAware<View_Main>,
	public Gc::Model::ListCfg::ListCfgConnection,
	public Gc::Model::SettingsManagerDataConnection,
	public Gc::Model::FbResolutionsGetterConnection,
	public Gc::Model::Device::DeviceDataListConnection,
	public Gc::Model::Device::MountTableConnection,
	public Gc::Model::ContentParser::GenericFactoryConnection,
	public Gc::View::Mapper::EntryNameConnection,
	public Gc::Model::EnvConnection,
	public Gc::Controller::Helper::ThreadConnection,
	public Gc::Bootstrap::ApplicationHelper::ObjectConnection,
	public Gc::Controller::Helper::RuleMoverConnection
{
	private: std::shared_ptr<Gc::Model::SettingsManagerData> settingsOnDisk; //buffer for the existing settings
	private: std::shared_ptr<Gc::Model::ListCfg::ListCfg> savedListCfg;
	private: Gc::Model::ContentParser::GenericParser* currentContentParser;

	private: bool config_has_been_different_on_startup_but_unsaved;
	private: bool is_loading;
	private: CmdExecException thrownException; //to be used from the die() function

	public: void setSettingsBuffer(std::shared_ptr<Gc::Model::SettingsManagerData> settings)
	{
		this->settingsOnDisk = settings;
	}

	public: void setSavedListCfg(std::shared_ptr<Gc::Model::ListCfg::ListCfg> savedListCfg)
	{
		this->savedListCfg = savedListCfg;
	}

	public: Gc::Model::FbResolutionsGetter& getFbResolutionsGetter() {
		return *this->fbResolutionsGetter;
	}

	public: void initViewEvents() override
	{
		using namespace std::placeholders;
		this->view->onRemoveRulesClick = std::bind(std::mem_fn(&MainController::removeRulesAction), this, _1, _2);
		this->view->onShowSettingsClick = std::bind(std::mem_fn(&MainController::showSettingsAction), this);
		this->view->onReloadClick = std::bind(std::mem_fn(&MainController::reloadAction), this);
		this->view->onSaveClick = std::bind(std::mem_fn(&MainController::saveAction), this);
		this->view->onShowEnvEditorClick = std::bind(std::mem_fn(&MainController::showEnvEditorAction), this);
		this->view->onShowInstallerClick = std::bind(std::mem_fn(&MainController::showInstallerAction), this);
		this->view->onCreateSubmenuClick = std::bind(std::mem_fn(&MainController::createSubmenuAction), this, _1);
		this->view->onRemoveSubmenuClick = std::bind(std::mem_fn(&MainController::removeSubmenuAction), this, _1);
		this->view->onShowEntryEditorClick = std::bind(std::mem_fn(&MainController::showEntryEditorAction), this, _1);
		this->view->onShowEntryCreatorClick = std::bind(std::mem_fn(&MainController::showEntryCreatorAction), this);
		this->view->onShowAboutClick = std::bind(std::mem_fn(&MainController::showAboutAction), this);
		this->view->onExitClick = std::bind(std::mem_fn(&MainController::exitAction), this);
		this->view->onRenameClick = std::bind(std::mem_fn(&MainController::renameRuleAction), this, _1, _2);
		this->view->onRevertClick = std::bind(std::mem_fn(&MainController::revertAction), this);
		this->view->onMoveClick = std::bind(std::mem_fn(&MainController::moveAction), this, _1, _2);
		this->view->onCancelBurgSwitcherClick = std::bind(std::mem_fn(&MainController::cancelBurgSwitcherAction), this);
		this->view->onInitModeClick = std::bind(std::mem_fn(&MainController::initModeAction), this, _1);
		this->view->onRuleSelection = std::bind(std::mem_fn(&MainController::selectRuleAction), this, _1, _2);
		this->view->onTabChange = std::bind(std::mem_fn(&MainController::refreshTabAction), this, _1);
		this->view->onViewOptionChange = std::bind(std::mem_fn(&MainController::setViewOptionAction), this, _1, _2);
		this->view->onEntryStateChange = std::bind(std::mem_fn(&MainController::entryStateToggledAction), this, _1, _2);
		this->view->onSelectionChange = std::bind(std::mem_fn(&MainController::updateSelectionAction), this, _1);
	}

	public: void initApplicationEvents() override
	{
		using namespace std::placeholders;

		this->applicationObject->addShutdownHandler(
			[this] () {
				this->view->setLockState(~0);
				if (this->mountTable->getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT)) {
					this->mountTable->umountAll(PARTCHOOSER_MOUNTPOINT);
				}
			}
		);

		this->applicationObject->onListModelChange.addHandler(
			std::bind(std::mem_fn(&MainController::updateList), this)
		);

		this->applicationObject->onListModelChange.addHandler(
			std::bind(std::mem_fn(&MainController::updateTrashView), this)
		);

		this->applicationObject->onEnvChange.addHandler(
			std::bind(std::mem_fn(&MainController::reInitAction), this, _1)
		);
		this->applicationObject->onListRelevantSettingChange.addHandler(
			std::bind(std::mem_fn(&MainController::showReloadRecommendationAction), this)
		);

		this->applicationObject->onListRuleChange.addHandler(
			std::bind(std::mem_fn(&MainController::selectRuleAction), this, _1, _2)
		);

		this->applicationObject->onTrashEntrySelection.addHandler(
			std::bind(std::mem_fn(&MainController::selectRulesAction), this, std::list<Gc::Common::Type::Rule*>())
		);

		this->applicationObject->onEntryInsertionRequest.addHandler(
			std::bind(std::mem_fn(&MainController::addEntriesAction), this, _1)
		);
	}

	public: void initListCfgEvents() override
	{
		using namespace std::placeholders;

		this->grublistCfg->onLoadStateChange = std::bind(std::mem_fn(&MainController::syncLoadStateThreadedAction), this);
		this->grublistCfg->onSaveStateChange = std::bind(std::mem_fn(&MainController::syncSaveStateThreadedAction), this);
	}

	//init functions
	public: void init()
	{
		if ( !grublistCfg
			or !view
			or !settings
			or !settingsOnDisk
			or !savedListCfg
			or !fbResolutionsGetter
			or !deviceDataList
			or !mountTable
			or !contentParserFactory
			or !threadHelper
			or !entryNameMapper
		) {
			throw ConfigException("init(): missing some objects", __FILE__, __LINE__);
		}
		this->log("initializing (w/o specified bootloader type)…", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);

		savedListCfg->verbose = false;

		this->log("reading partition info…", Gc::Model::Logger::GenericLogger::EVENT);
		FILE* blkidProc = popen("blkid", "r");
		if (blkidProc){
			deviceDataList->clear();
			deviceDataList->loadData(blkidProc);
			pclose(blkidProc);
		}

		mountTable->loadData("");
		mountTable->loadData(PARTCHOOSER_MOUNTPOINT);


		this->env->rootDeviceName = mountTable->getEntryByMountpoint("").device;

		this->applicationObject->onInit.exec();

		//dir_prefix may be set by partition chooser (if not, the root partition is used)

		this->log("Finding out if this is a live CD", Gc::Model::Logger::GenericLogger::EVENT);
		//aufs is the virtual root fileSystem used by live cds
		if (mountTable->getEntryByMountpoint("").isLiveCdFs() && env->cfg_dir_prefix == ""){
			this->log("is live CD", Gc::Model::Logger::GenericLogger::INFO);
			this->env->init(Gc::Model::Env::Mode::GRUB, "");
			this->showEnvEditorAction();
		} else {
			this->log("running on an installed system", Gc::Model::Logger::GenericLogger::INFO);
			std::list<Gc::Model::Env::Mode> modes = this->env->getAvailableModes();
			if (modes.size() == 2) {
				this->view->showBurgSwitcher();
			} else if (modes.size() == 1) {
				this->init(modes.front());
			} else if (modes.size() == 0) {
				this->showEnvEditorAction();
			}
		}
	}

	public: void init(Gc::Model::Env::Mode mode, bool initEnv = true)
	{
		this->log("initializing (w/ specified bootloader type)…", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
		if (initEnv) {
			this->env->init(mode, env->cfg_dir_prefix);
		}
		this->view->setLockState(1|4|8);
		this->view->setIsBurgMode(mode == Gc::Model::Env::Mode::BURG);
		this->view->show();
		this->view->hideBurgSwitcher();
		this->view->hideScriptUpdateInfo();

		this->log("Checking if the config directory is clean", Gc::Model::Logger::GenericLogger::EVENT);
		if (this->grublistCfg->cfgDirIsClean() == false) {
			this->log("cleaning up config dir", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
			this->grublistCfg->cleanupCfgDir();
		}

		this->log("loading configuration", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
		this->threadHelper->runAsThread(std::bind(std::mem_fn(&MainController::loadThreadedAction), this, false));
	}

	public: void initAction()
	{
		this->logActionBegin("init");
		try {
			this->init();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void reInitAction(bool burgMode)
	{
		this->logActionBegin("re-init");
		try {
			Gc::Model::Env::Mode mode = burgMode ? Gc::Model::Env::Mode::BURG : Gc::Model::Env::Mode::GRUB;
			this->init(mode, false);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void showEnvEditorAction()
	{
		this->logActionBegin("show-env-editor");
		try {
			if (this->env->modificationsUnsaved) {
				bool proceed = this->view->confirmUnsavedSwitch();
				if (!proceed) {
					return;
				}
			}

			this->view->hide();

			this->applicationObject->onEnvEditorShowRequest.exec();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void cancelBurgSwitcherAction()
	{
		this->logActionBegin("cancel-burg-switcher");
		try {
			if (!this->view->isVisible()) {
				this->applicationObject->shutdown();
			}
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}


	public: void reloadAction()
	{
		this->logActionBegin("reload");
		try {
			this->applicationObject->onSettingModelChange.exec();
			this->view->hideReloadRecommendation();
			this->view->setLockState(1|4|8);
			this->threadHelper->runAsThread(std::bind(std::mem_fn(&MainController::loadThreadedAction), this, true));
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void loadThreadedAction(bool preserveConfig)
	{
		this->logActionBeginThreaded("load-threaded");
		try {
			if (!is_loading){ //allow only one load thread at the same time!
				this->log(std::string("loading - preserveConfig: ") + (preserveConfig ? "yes" : "no"), Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
				is_loading = true;
				this->env->activeThreadCount++;

				try {
					this->view->setOptions(this->env->loadViewOptions());
				} catch (FileReadException e) {
					this->log("view options not found", Gc::Model::Logger::GenericLogger::INFO);
				}
				this->applicationObject->viewOptions = this->view->getOptions();

				if (!preserveConfig){
					this->log("unsetting saved config", Gc::Model::Logger::GenericLogger::EVENT);
					this->grublistCfg->reset();
					this->savedListCfg->reset();
					//load the burg/grub settings file
					this->log("loading settings", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
					this->settings->load();
					this->threadHelper->runDispatched(std::bind(std::mem_fn(&MainController::activateSettingsAction), this));
				} else {
					this->log("switching settings", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
					this->settingsOnDisk->load();
					this->settings->save();
				}

				try {
					this->log("loading grub list", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
					this->grublistCfg->load(preserveConfig);
					this->log("grub list completely loaded", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
				} catch (CmdExecException const& e){
					this->log("error while loading the grub list", Gc::Model::Logger::GenericLogger::ERROR);
					this->thrownException = e;
					this->threadHelper->runDispatched(std::bind(std::mem_fn(&MainController::dieAction), this));
					return; //cancel
				}

				if (!preserveConfig){
					this->log("loading saved grub list", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
					if (this->savedListCfg->loadStaticCfg()) {
						this->config_has_been_different_on_startup_but_unsaved = !this->grublistCfg->compare(*this->savedListCfg);
					} else {
						this->log("saved grub list not found", Gc::Model::Logger::GenericLogger::WARNING);
						this->config_has_been_different_on_startup_but_unsaved = false;
					}
					this->threadHelper->runDispatched([this] {this->applicationObject->onLoad.exec();});
				}
				if (preserveConfig){
					this->log("restoring settings", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
					this->settingsOnDisk->save();
				}
				this->env->activeThreadCount--;
				this->is_loading = false;
			} else {
				this->log("ignoring load request (only one load thread allowed at the same time)", Gc::Model::Logger::GenericLogger::WARNING);
			}
		} catch (Exception const& e) {
			this->applicationObject->onThreadError.exec(e);
		}
		this->logActionEndThreaded();
	}

	public: void saveAction()
	{
		this->logActionBegin("save");
		try {
			this->config_has_been_different_on_startup_but_unsaved = false;
			this->env->modificationsUnsaved = false; //deprecated
			this->view->hideScriptUpdateInfo();

			this->view->setLockState(1|4|8);
			this->env->activeThreadCount++; //not in save_thead() to be faster set
			this->threadHelper->runAsThread(std::bind(std::mem_fn(&MainController::saveThreadedAction), this));
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void saveThreadedAction()
	{
		this->logActionBeginThreaded("save-threaded");
		try {
			this->env->createBackup();
			this->log("writing settings file", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
			this->settings->save();
			if (this->settings->color_helper_required) {
				this->grublistCfg->addColorHelper();
			}
			this->applicationObject->onSave.exec();
			this->log("writing grub list configuration", Gc::Model::Logger::GenericLogger::IMPORTANT_EVENT);
			try {
				this->grublistCfg->save();
			} catch (CmdExecException const& e){
				this->threadHelper->runDispatched(std::bind(std::mem_fn(&MainController::showConfigSavingErrorAction), this, e.getMessage()));
			}
			this->env->activeThreadCount--;
		} catch (Exception const& e) {
			this->applicationObject->onThreadError.exec(e);
		}
		this->logActionEndThreaded();
	}

	public: void showConfigSavingErrorAction(std::string errorMessage)
	{
		this->logActionBeginThreaded("show-config-saving-error");
		try {
			this->view->showConfigSavingError(errorMessage);
		} catch (Exception const& e) {
			this->applicationObject->onThreadError.exec(e);
		}
		this->logActionEndThreaded();
	}

	public: MainController() :
		Gc::Controller::Common::ControllerAbstract("main"),
		config_has_been_different_on_startup_but_unsaved(false),
		is_loading(false),
		currentContentParser(NULL),
		thrownException("")
	{
	}

	public: void renameEntry(std::shared_ptr<Gc::Model::ListCfg::Rule> rule, std::string const& newName)
	{
		if (rule->type != Gc::Model::ListCfg::Rule::PLAINTEXT) {

			std::string currentRulePath = this->grublistCfg->getRulePath(rule);
			std::string currentDefaultRulePath = this->settings->getValue("GRUB_DEFAULT");
			bool updateDefault = this->ruleAffectsCurrentDefaultOs(rule, currentRulePath, currentDefaultRulePath);

			this->grublistCfg->renameRule(rule, newName);

			if (updateDefault) {
				this->updateCurrentDefaultOs(rule, currentRulePath, currentDefaultRulePath);
			}

			if (rule->dataSource && this->grublistCfg->repository.getScriptByEntry(rule->dataSource)->isCustomScript) {
				rule->dataSource->name = newName;
			}

			this->applicationObject->onListModelChange.exec();
			this->view->selectRule(rule.get());
		}
	}

	public: void reset()
	{
		this->grublistCfg->reset();
		this->settings->clear();
	}


	public: void showInstallerAction()
	{
		this->logActionBegin("show-installer");
		try {
			this->applicationObject->onInstallerShowRequest.exec();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}


	public: void showEntryEditorAction(Gc::Common::Type::Rule* rule)
	{
		this->logActionBegin("show-entry-editor");
		try {
			this->applicationObject->onEntryEditorShowRequest.exec(rule);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void showEntryCreatorAction()
	{
		this->logActionBegin("show-entry-creator");
		try {
			this->applicationObject->onEntryEditorShowRequest.exec(nullptr);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}


	//dispatchers
	public: void dieAction()
	{
		this->logActionBegin("die");
		try {
			this->is_loading = false;
			this->env->activeThreadCount = 0;
			bool showEnvSettings = false;
			if (this->thrownException){
				showEnvSettings = this->view->askForEnvironmentSettings(this->env->mkconfig_cmd, this->grublistCfg->getGrubErrorMessage());
			}
			if (showEnvSettings) {
				this->showEnvEditorAction();
			} else {
				this->applicationObject->shutdown();
			}
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void updateList()
	{
		this->view->clear();

		for (auto& proxy : this->grublistCfg->proxies){
			std::string name = proxy->getScriptName();
			if ((name != "header" && name != "debian_theme" && name != "grub-customizer_menu_color_helper") || proxy->isModified()) {
				View_Model_ListItem<Gc::Common::Type::Rule, Gc::Common::Type::Proxy> listItem;
				listItem.name = name;
				listItem.scriptPtr = proxy.get();
				listItem.is_submenu = true;
				listItem.defaultName = name;
				listItem.isVisible = true;
				this->view->appendEntry(listItem);
				for (auto& rule : proxy->rules){
					this->appendRuleToView(rule);
				}
			}
		}
	}

	public: void updateTrashView()
	{
		bool placeholdersVisible = this->view->getOptions().at(Gc::Common::Type::ViewOption::SHOW_PLACEHOLDERS);
		bool hiddenEntriesVisible = this->view->getOptions().at(Gc::Common::Type::ViewOption::SHOW_HIDDEN_ENTRIES);
		this->view->setTrashPaneVisibility(
			this->grublistCfg->getRemovedEntries(NULL, !placeholdersVisible).size() >= 1 && !hiddenEntriesVisible
		);
	}


	public: void exitAction()
	{
		this->logActionBegin("exit");
		try {
			int dlgResponse = this->view->showExitConfirmDialog(this->config_has_been_different_on_startup_but_unsaved*2 + this->env->modificationsUnsaved);
			if (dlgResponse == 1){
				this->saveAction(); //starts a thread that delays the application exiting
			}

			if (dlgResponse != 0){
				if (this->env->activeThreadCount != 0){
					this->env->quit_requested = true;
					this->grublistCfg->cancelThreads();
				}
				else {
					this->applicationObject->shutdown();
				}
			}
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}


	public: void removeRulesAction(std::list<Gc::Common::Type::Rule*> rules, bool force)
	{
		this->logActionBegin("remove-rules");
		try {
			if (!force && this->listHasAllCurrentSystemRules(rules)) {
				this->view->showSystemRuleRemoveWarning();
			} else if (!force && this->listHasPlaintextRules(rules)) {
				this->view->showPlaintextRemoveWarning();
			} else {
				std::list<Gc::Common::Type::Entry*> entriesOfRemovedRules;
				std::map<std::shared_ptr<Gc::Model::ListCfg::Proxy>, Gc::Common::Type::Nothing> emptyProxies;
				for (std::list<Gc::Common::Type::Rule*>::iterator iter = rules.begin(); iter != rules.end(); iter++) {
					std::shared_ptr<Gc::Model::ListCfg::Rule> rule = this->grublistCfg->findRule(*iter);
					rule->setVisibility(false);
					entriesOfRemovedRules.push_back(rule->dataSource.get());
					if (!this->grublistCfg->proxies.getProxyByRule(rule)->hasVisibleRules()) {
						emptyProxies[this->grublistCfg->proxies.getProxyByRule(rule)] = Gc::Common::Type::Nothing();
					}
				}

				for (auto emptyProxy : emptyProxies) {
					this->grublistCfg->proxies.deleteProxy(emptyProxy.first);
					this->log("proxy removed", Gc::Model::Logger::GenericLogger::INFO);
				}

				this->applicationObject->onListModelChange.exec();

				this->applicationObject->onEntryRemove.exec(entriesOfRemovedRules);
				this->env->modificationsUnsaved = true;
			}
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void renameRuleAction(Gc::Common::Type::Rule* entry, std::string const& newText)
	{
		this->logActionBegin("rename-rule");
		try {
			auto entry2 = this->grublistCfg->findRule(entry);
			std::string oldName = entry2->outputName;
			if (newText == ""){
				this->view->showErrorMessage(gettext("Name the Entry"));
				this->view->setRuleName(entry, oldName);
			}
			else {
				this->renameEntry(entry2, newText);
			}
			this->env->modificationsUnsaved = true;
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void moveAction(std::list<Gc::Common::Type::Rule*> rules, int direction)
	{
		this->logActionBegin("move");
		try {
			bool stickyPlaceholders = !this->view->getOptions().at(Gc::Common::Type::ViewOption::SHOW_PLACEHOLDERS);
			try {
				assert(direction == -1 || direction == 1);

				if (stickyPlaceholders) {
					auto nextRealRule = this->findNextRealRule(this->grublistCfg->findRule(direction == -1 ? rules.front() : rules.back()), direction);
					rules = this->populateSelection(rules, nextRealRule->type == Gc::Model::ListCfg::Rule::SUBMENU);
					rules = this->grublistCfg->getNormalizedRuleOrder(rules);
				}

				if (direction == 1) {
					rules.reverse();
				}

				for (auto& rulePtr : rules) { // move multiple rules
					auto rule = this->grublistCfg->findRule(rulePtr);
					int distance = 1;
					if (stickyPlaceholders) {
						distance = this->countRulesUntilNextRealRule(rule, direction);
					}

					std::string currentRulePath = this->grublistCfg->getRulePath(rule);
					std::string currentDefaultRulePath = this->settings->getValue("GRUB_DEFAULT");
					bool updateDefault = this->ruleAffectsCurrentDefaultOs(rule, currentRulePath, currentDefaultRulePath);

					for (int j = 0; j < distance; j++) { // move the range multiple times
						this->ruleMover->move(
							rule,
							direction == -1 ? Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::UP : Gc::Controller::Helper::RuleMover::AbstractStrategy::Direction::DOWN
						);
					}

					if (updateDefault) {
						this->updateCurrentDefaultOs(rule, currentRulePath, currentDefaultRulePath);
					}
				}

				this->applicationObject->onListModelChange.exec();
				if (stickyPlaceholders) {
					rules = this->removePlaceholdersFromSelection(rules);
				}
				this->view->selectRules(rules);
				this->env->modificationsUnsaved = true;
			} catch (NoMoveTargetException const& e) {
				this->view->showErrorMessage(gettext("cannot move this entry"));
				this->applicationObject->onListModelChange.exec();
			}
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void createSubmenuAction(std::list<Gc::Common::Type::Rule*> childItems)
	{
		this->logActionBegin("create-submenu");
		try {
			auto firstRule = this->grublistCfg->findRule(childItems.front());
			auto newItem = this->grublistCfg->createSubmenu(firstRule);
			this->applicationObject->onListModelChange.exec();
			this->moveAction(childItems, -1);
			this->threadHelper->runDelayed(
				std::bind(std::mem_fn(&MainController::selectRuleAction), this, newItem.get(), true),
				10
			);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void removeSubmenuAction(std::list<Gc::Common::Type::Rule*> childItems)
	{
		this->logActionBegin("remove-submenu");
		try {
			auto firstItem = this->grublistCfg->splitSubmenu(this->grublistCfg->findRule(childItems.front()));
			std::list<Gc::Common::Type::Rule*> movedRules;
			movedRules.push_back(firstItem.get());
			for (int i = 1; i < childItems.size(); i++) {
				movedRules.push_back(this->grublistCfg->proxies.getNextVisibleRule(this->grublistCfg->findRule(movedRules.back()), 1)->get());
			}

			this->moveAction(movedRules, -1);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}


	public: void revertAction()
	{
		this->logActionBegin("revert");
		try {
			this->grublistCfg->revert();
			this->applicationObject->onListModelChange.exec();
			this->env->modificationsUnsaved = true;
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}


	public: void showProxyInfo(Gc::Model::ListCfg::Proxy* proxy)
	{
		this->view->setStatusText("");
	}


	public: void showAboutAction()
	{
		this->logActionBegin("show-about");
		try {
			this->applicationObject->onAboutDlgShowRequest.exec();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}


	public: void syncLoadStateThreadedAction()
	{
		this->logActionBeginThreaded("sync-load-state-threaded");
		try {
			this->threadHelper->runDispatched(std::bind(std::mem_fn(&MainController::syncLoadStateAction), this));
		} catch (Exception const& e) {
			this->applicationObject->onThreadError.exec(e);
		}
		this->logActionEndThreaded();
	}

	public: void syncSaveStateThreadedAction()
	{
		this->logActionBeginThreaded("sync-save-state-threaded");
		try {
			this->threadHelper->runDispatched(std::bind(std::mem_fn(&MainController::syncSaveStateAction), this));
		} catch (Exception const& e) {
			this->applicationObject->onThreadError.exec(e);
		}
		this->logActionEndThreaded();
	}


	public: void syncSaveStateAction()
	{
		this->logActionBegin("sync-save-state");
		try {
			this->log("running MainControllerImpl::syncListView_save", Gc::Model::Logger::GenericLogger::INFO);
			this->view->progress_pulse();
			if (this->grublistCfg->getProgress() == 1){
				if (this->grublistCfg->error_proxy_not_found){
					this->view->showProxyNotFoundMessage();
					this->grublistCfg->error_proxy_not_found = false;
				}
				if (this->env->quit_requested) {
					this->applicationObject->shutdown();
				}

				this->view->setLockState(0);

				this->view->hideProgressBar();
				this->view->setStatusText(gettext("Configuration has been saved"));

				this->updateList();
			}
			else {
				this->view->setStatusText(gettext("updating configuration"));
			}
			this->log("MainControllerImpl::syncListView_save completed", Gc::Model::Logger::GenericLogger::INFO);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void syncLoadStateAction()
	{
		this->logActionBegin("sync-load-state");
		try {
			this->log("running MainControllerImpl::syncListView_load", Gc::Model::Logger::GenericLogger::INFO);
			this->view->setLockState(1|4);
			double progress = this->grublistCfg->getProgress();
			if (progress != 1) {
				this->view->setProgress(progress);
				this->view->setStatusText(this->grublistCfg->getProgress_name(), this->grublistCfg->getProgress_pos(), this->grublistCfg->getProgress_max());
			} else {
				if (this->env->quit_requested) {
					this->applicationObject->shutdown();
				}
				this->view->hideProgressBar();
				this->view->setStatusText("");
			}

			if (progress == 1 && this->grublistCfg->hasScriptUpdates()) {
				this->grublistCfg->applyScriptUpdates();
				this->env->modificationsUnsaved = true;
				this->view->showScriptUpdateInfo();
			}

			//if grubConfig is locked, it will be cancelled as early as possible
			if (this->grublistCfg->lock_if_free()) {
				this->updateList();
				this->grublistCfg->unlock();
			}

			if (progress == 1){
				this->view->setLockState(0);

				this->applicationObject->onListModelChange.exec();
			}
			this->log("MainControllerImpl::syncListView_load completed", Gc::Model::Logger::GenericLogger::INFO);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void showSettingsAction()
	{
		this->logActionBegin("show-settings");
		try {
			this->applicationObject->onSettingsShowRequest.exec();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void initModeAction(bool burgChosen)
	{
		this->logActionBegin("init-mode");
		try {
			this->init(burgChosen ? Gc::Model::Env::Mode::BURG : Gc::Model::Env::Mode::GRUB);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void addEntriesAction(std::list<Gc::Common::Type::Rule*> rulePtrs)
	{
		this->logActionBegin("add-entries");
		try {
			std::list<Gc::Common::Type::Rule*> addedRules;
			for (auto rulePtr : rulePtrs) {
				auto& modelRule = dynamic_cast<Gc::Model::ListCfg::Rule&>(*rulePtr);
				auto entry = modelRule.dataSource;
				assert(entry != nullptr);
				addedRules.push_back(this->grublistCfg->addEntry(entry, modelRule.type == Gc::Model::ListCfg::Rule::OTHER_ENTRIES_PLACEHOLDER).get());
			}

			this->applicationObject->onListModelChange.exec();

			this->view->selectRules(addedRules);

			this->env->modificationsUnsaved = true;
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void activateSettingsAction()
	{
		this->logActionBegin("activate-settings");
		try {
			this->view->setLockState(1);
			this->applicationObject->onSettingModelChange.exec();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void showReloadRecommendationAction()
	{
		this->logActionBegin("show-reload-recommendation");
		try {
			this->view->showReloadRecommendation();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void selectRulesAction(std::list<Gc::Common::Type::Rule*> rules)
	{
		this->logActionBegin("select-rules");
		try {
			this->view->selectRules(rules);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void selectRuleAction(Gc::Common::Type::Rule* rule, bool startEdit)
	{
		this->logActionBegin("select-rule");
		try {
			this->view->selectRule(rule, startEdit);
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void refreshTabAction(unsigned int pos)
	{
		this->logActionBegin("refresh-tab");
		try {
			if (pos != 0) { // list
				this->applicationObject->onSettingModelChange.exec();
			}
			this->view->updateLockState();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void setViewOptionAction(Gc::Common::Type::ViewOption option, bool value)
	{
		this->logActionBegin("set-view-option");
		try {
			this->view->setOption(option, value);
			try {
				this->applicationObject->viewOptions = this->view->getOptions();
				this->env->saveViewOptions(this->view->getOptions());
			} catch (FileSaveException e) {
				this->log("option saving failed", Gc::Model::Logger::GenericLogger::ERROR);
			}
			this->applicationObject->onListModelChange.exec();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void entryStateToggledAction(Gc::Common::Type::Rule* entry, bool state)
	{
		this->logActionBegin("entry-state-toggled");
		try {
			this->grublistCfg->findRule(entry)->setVisibility(state);
			this->applicationObject->onListModelChange.exec();
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	public: void updateSelectionAction(std::list<Gc::Common::Type::Rule*> selectedRules)
	{
		this->logActionBegin("update-selection");
		try {
			if (selectedRules.size()) {
				this->applicationObject->onEntrySelection.exec();
			}
		} catch (Exception const& e) {
			this->applicationObject->onError.exec(e);
		}
		this->logActionEnd();
	}

	private: void appendRuleToView(std::shared_ptr<Gc::Model::ListCfg::Rule> rule, std::shared_ptr<Gc::Model::ListCfg::Rule> parentRule = nullptr)
	{
		bool is_other_entries_ph = rule->type == Gc::Model::ListCfg::Rule::OTHER_ENTRIES_PLACEHOLDER;
		bool is_plaintext = rule->dataSource && rule->dataSource->type == Gc::Model::ListCfg::Entry::PLAINTEXT;
		bool is_submenu = rule->type == Gc::Model::ListCfg::Rule::SUBMENU;

		if (rule->dataSource || is_submenu){
			std::string name = this->entryNameMapper->map(rule->dataSource, rule->outputName, true);

			bool isSubmenu = rule->type == Gc::Model::ListCfg::Rule::SUBMENU;
			std::string scriptName = "", defaultName = "";
			if (rule->dataSource) {
				auto script = this->grublistCfg->repository.getScriptByEntry(rule->dataSource);
				assert(script != nullptr);
				scriptName = script->name;
				if (!is_other_entries_ph && !is_plaintext) {
					defaultName = rule->dataSource->name;
				}
			}
			bool isEditable = rule->type == Gc::Model::ListCfg::Rule::NORMAL || rule->type == Gc::Model::ListCfg::Rule::PLAINTEXT;
			bool isModified = rule->dataSource && rule->dataSource->isModified;

			// parse content to show additional informations
			std::map<std::string, std::string> options;
			if (rule->dataSource) {
				options = Gc::Controller::Helper::DeviceInfo::fetch(rule->dataSource->content, *contentParserFactory, *deviceDataList);
			}

			auto proxy = this->grublistCfg->proxies.getProxyByRule(rule);

			View_Model_ListItem<Gc::Common::Type::Rule, Gc::Common::Type::Proxy> listItem;
			listItem.name = name;
			listItem.entryPtr = rule.get();
			listItem.is_placeholder = is_other_entries_ph || is_plaintext;
			listItem.is_submenu = isSubmenu;
			listItem.scriptName = scriptName;
			listItem.defaultName = defaultName;
			listItem.isEditable = isEditable;
			listItem.isModified = isModified;
			listItem.options = options;
			listItem.isVisible = rule->isVisible;
			listItem.parentEntry = parentRule.get();
			listItem.parentScript = proxy.get();
			this->view->appendEntry(listItem);

			if (rule->type == Gc::Model::ListCfg::Rule::SUBMENU) {
				for (auto subRule : rule->subRules) {
					this->appendRuleToView(subRule, rule);
				}
			}
		}
	}

	private: bool listHasPlaintextRules(std::list<Gc::Common::Type::Rule*> const& rules)
	{
		for (auto rulePtr : rules) {
			auto rule = this->grublistCfg->findRule(rulePtr);
			if (rule->type == Gc::Model::ListCfg::Rule::PLAINTEXT) {
				return true;
			}
		}
		return false;
	}

	private: bool listHasAllCurrentSystemRules(std::list<Gc::Common::Type::Rule*> const& rules)
	{
		int visibleSystemRulesCount = 0;
		int selectedSystemRulesCount = 0;

		std::shared_ptr<Gc::Model::ListCfg::Script> linuxScript = nullptr;

		// count selected entries related to linux script
		for (auto rulePtr : rules) {
			auto rule = this->grublistCfg->findRule(rulePtr);
			if (rule->type == Gc::Model::ListCfg::Rule::NORMAL) {
				assert(rule->dataSource != nullptr);
				auto script = this->grublistCfg->repository.getScriptByEntry(rule->dataSource);
				if (script->name == "linux") {
					selectedSystemRulesCount++;

					linuxScript = script;
				}
			}
		}

		// count all entries and compare counters if there are linux entries
		if (linuxScript != nullptr) {
			// check whether it's the last remaining entry
			auto proxies = this->grublistCfg->proxies.getProxiesByScript(linuxScript);
			bool visibleRulesFound = false;
			for (auto proxy : proxies) {
				visibleSystemRulesCount += proxy->getVisibleRulesByType(Gc::Model::ListCfg::Rule::NORMAL).size();
			}

			if (selectedSystemRulesCount == visibleSystemRulesCount) {
				return true;
			}
		}

		return false;
	}

	private: std::list<Gc::Common::Type::Rule*> populateSelection(std::list<Gc::Common::Type::Rule*> rules, bool ignorePlaintext)
	{
		std::list<Gc::Common::Type::Rule*> result;
		for (auto rule : rules) {
			this->populateSelection(result, this->grublistCfg->findRule(rule), -1, rule == rules.front(), ignorePlaintext);
			result.push_back(rule);
			this->populateSelection(result, this->grublistCfg->findRule(rule), 1, rule == rules.back(), ignorePlaintext);
		}
		// remove duplicates
		std::list<Gc::Common::Type::Rule*> result2;
		std::map<Gc::Common::Type::Rule*, Gc::Common::Type::Rule*> duplicateIndex; // key: pointer to the rule, value: always NULL
		for (std::list<Gc::Common::Type::Rule*>::iterator ruleIter = result.begin(); ruleIter != result.end(); ruleIter++) {
			if (duplicateIndex.find(*ruleIter) == duplicateIndex.end()) {
				duplicateIndex[*ruleIter] = nullptr;
				result2.push_back(*ruleIter);
			}
		}
		return result2;
	}

	private: void populateSelection(std::list<Gc::Common::Type::Rule*>& rules, std::shared_ptr<Gc::Model::ListCfg::Rule> baseRule, int direction, bool checkScript, bool ignorePlaintext)
	{
		assert(direction == 1 || direction == -1);
		bool placeholderFound = false;
		auto currentRule = baseRule;
		do {
			try {
				currentRule = *this->grublistCfg->proxies.getNextVisibleRule(currentRule, direction);
				if (currentRule->dataSource == nullptr || baseRule->dataSource == nullptr) {
					break;
				}
				auto scriptCurrent = this->grublistCfg->repository.getScriptByEntry(currentRule->dataSource);
				auto scriptBase    = this->grublistCfg->repository.getScriptByEntry(baseRule->dataSource);

				if ((scriptCurrent == scriptBase || !checkScript) && (currentRule->type == Gc::Model::ListCfg::Rule::OTHER_ENTRIES_PLACEHOLDER || (currentRule->type == Gc::Model::ListCfg::Rule::PLAINTEXT && !ignorePlaintext))) {
					if (direction == 1) {
						rules.push_back(currentRule.get());
					} else {
						rules.push_front(currentRule.get());
					}
					placeholderFound = true;
				} else {
					placeholderFound = false;
				}
			} catch (NoMoveTargetException const& e) {
				placeholderFound = false;
			}
		} while (placeholderFound);
	}

	private: int countRulesUntilNextRealRule(std::shared_ptr<Gc::Model::ListCfg::Rule> baseRule, int direction)
	{
		int result = 1;
		bool placeholderFound = false;
		auto currentRule = baseRule;
		do {
			try {
				currentRule = *this->grublistCfg->proxies.getNextVisibleRule(currentRule, direction);

				if (currentRule->type == Gc::Model::ListCfg::Rule::OTHER_ENTRIES_PLACEHOLDER || currentRule->type == Gc::Model::ListCfg::Rule::PLAINTEXT) {
					result++;
					placeholderFound = true;
				} else {
					placeholderFound = false;
				}
			} catch (NoMoveTargetException const& e) {
				placeholderFound = false;
			}
		} while (placeholderFound);
		return result;
	}

	private: std::shared_ptr<Gc::Model::ListCfg::Rule> findNextRealRule(std::shared_ptr<Gc::Model::ListCfg::Rule> baseRule, int direction)
	{
		bool placeholderFound = false;
		auto currentRule = baseRule;
		do {
			try {
				currentRule = *this->grublistCfg->proxies.getNextVisibleRule(currentRule, direction);

				if (currentRule->type == Gc::Model::ListCfg::Rule::OTHER_ENTRIES_PLACEHOLDER || currentRule->type == Gc::Model::ListCfg::Rule::PLAINTEXT) {
					placeholderFound = true;
				} else {
					placeholderFound = false;
				}
			} catch (NoMoveTargetException const& e) {
				placeholderFound = false;
			}
		} while (placeholderFound);

		return currentRule;
	}

	private: std::list<Gc::Common::Type::Rule*> removePlaceholdersFromSelection(std::list<Gc::Common::Type::Rule*> rules)
	{
		std::list<Gc::Common::Type::Rule*> result;
		for (auto rulePtr : rules) {
			auto rule = this->grublistCfg->findRule(rulePtr);
			if (!(rule->type == Gc::Model::ListCfg::Rule::OTHER_ENTRIES_PLACEHOLDER || rule->type == Gc::Model::ListCfg::Rule::PLAINTEXT)) {
				result.push_back(rule.get());
			}
		}
		return result;
	}

	private: bool ruleAffectsCurrentDefaultOs(
		std::shared_ptr<Gc::Model::ListCfg::Rule> rule,
		std::string const& currentRulePath,
		std::string const& currentDefaultRulePath
	)
	{
		bool result = false;

		if (rule->type == Gc::Model::ListCfg::Rule::SUBMENU) {
			if (currentDefaultRulePath.substr(0, currentRulePath.length() + 1) == currentRulePath + ">") {
				result = true;
			}
		} else {
			if (this->settings->getValue("GRUB_DEFAULT") == currentRulePath) {
				result = true;
			}
		}
		return result;
	}

	private: void updateCurrentDefaultOs(std::shared_ptr<Gc::Model::ListCfg::Rule> rule, std::string const& oldRulePath, std::string oldDefaultRulePath) {
		oldDefaultRulePath.replace(0, oldRulePath.length(), this->grublistCfg->getRulePath(rule));
		this->settings->setValue("GRUB_DEFAULT", oldDefaultRulePath);
	}
};}}

#endif
