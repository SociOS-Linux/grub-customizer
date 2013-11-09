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
#include "RuleMove.h"

void Controller_Helper_RuleMove::setDefaultOsUpdater(Controller_Helper_DefaultOsUpdater defaultOsUpdater) {
	this->defaultOsUpdater = defaultOsUpdater;
}

void Controller_Helper_RuleMove::move(std::list<Rule*> rules, int direction) {
	bool stickyPlaceholders = !this->view->getOptions().at(VIEW_SHOW_PLACEHOLDERS);
	try {
		assert(direction == -1 || direction == 1);
		int distance = 1;
		if (stickyPlaceholders) {
			rules = this->_populateSelection(rules);
			rules = this->grublistCfg->getNormalizedRuleOrder(rules);
			distance = this->_countRulesUntilNextRealRule(&Model_Rule::fromPtr(direction == -1 ? rules.front() : rules.back()), direction);
		}

		std::list<Rule*> movedRules;

		for (int j = 0; j < distance; j++) { // move the range multiple times
			int ruleCount = rules.size();
			Model_Rule* rulePtr = &Model_Rule::fromPtr(direction == -1 ? rules.front() : rules.back());
			for (int i = 0; i < ruleCount; i++) { // move multiple rules
				std::string currentRulePath = this->grublistCfg->getRulePath(*rulePtr);
				std::string currentDefaultRulePath = this->settings->getValue("GRUB_DEFAULT");
				bool updateDefault = this->defaultOsUpdater.ruleAffectsCurrentDefaultOs(rulePtr, currentRulePath, currentDefaultRulePath);

				rulePtr = &this->grublistCfg->moveRule(rulePtr, direction);

				if (updateDefault) {
					this->defaultOsUpdater.updateCurrentDefaultOs(rulePtr, currentRulePath, currentDefaultRulePath);
				}

				if (i < ruleCount - 1) {
					bool isEndOfList = false;
					bool targetFound = false;
					try {
						rulePtr = &*this->grublistCfg->proxies.getNextVisibleRule(rulePtr, -direction);
					} catch (NoMoveTargetException const& e) {
						isEndOfList = true;
						rulePtr = this->grublistCfg->proxies.getProxyByRule(rulePtr)->getParentRule(rulePtr);
					}
					if (!isEndOfList && rulePtr->type == Model_Rule::SUBMENU) {
						rulePtr = direction == -1 ? &rulePtr->subRules.front() : &rulePtr->subRules.back();
						if (rulePtr->isVisible) {
							targetFound = true;
						}
					}

					if (!targetFound) {
						rulePtr = &*this->grublistCfg->proxies.getNextVisibleRule(rulePtr, -direction);
					}
				}
			}

			movedRules.clear();
			movedRules.push_back(rulePtr);
			for (int i = 1; i < ruleCount; i++) {
				movedRules.push_back(&*this->grublistCfg->proxies.getNextVisibleRule(&Model_Rule::fromPtr(movedRules.back()), direction));
			}
			movedRules = this->grublistCfg->getNormalizedRuleOrder(movedRules);

			rules = movedRules;
		}

		this->controller->syncLoadStateAction();
		if (stickyPlaceholders) {
			movedRules = this->_removePlaceholdersFromSelection(movedRules);
		}
		this->view->selectRules(movedRules);
		this->env->modificationsUnsaved = true;
	} catch (NoMoveTargetException const& e) {
		this->view->showErrorMessage(gettext("cannot move this entry"));
		this->controller->syncLoadStateAction();
	}
}

std::list<Rule*> Controller_Helper_RuleMove::_populateSelection(std::list<Rule*> rules) {
	std::list<Rule*> result;
	for (std::list<Rule*>::iterator ruleIter = rules.begin(); ruleIter != rules.end(); ruleIter++) {
		this->_populateSelection(result, &Model_Rule::fromPtr(*ruleIter), -1, *ruleIter == rules.front());
		result.push_back(*ruleIter);
		this->_populateSelection(result, &Model_Rule::fromPtr(*ruleIter), 1, *ruleIter == rules.back());
	}
	// remove duplicates
	std::list<Rule*> result2;
	std::map<Rule*, Rule*> duplicateIndex; // key: pointer to the rule, value: always NULL
	for (std::list<Rule*>::iterator ruleIter = result.begin(); ruleIter != result.end(); ruleIter++) {
		if (duplicateIndex.find(*ruleIter) == duplicateIndex.end()) {
			duplicateIndex[*ruleIter] = NULL;
			result2.push_back(*ruleIter);
		}
	}
	return result2;
}

void Controller_Helper_RuleMove::_populateSelection(std::list<Rule*>& rules, Model_Rule* baseRule, int direction, bool checkScript) {
	assert(direction == 1 || direction == -1);
	bool placeholderFound = false;
	Model_Rule* currentRule = baseRule;
	do {
		try {
			currentRule = &*this->grublistCfg->proxies.getNextVisibleRule(currentRule, direction);
			if (currentRule->dataSource == NULL || baseRule->dataSource == NULL) {
				break;
			}
			Model_Script* scriptCurrent = this->grublistCfg->repository.getScriptByEntry(*currentRule->dataSource);
			Model_Script* scriptBase    = this->grublistCfg->repository.getScriptByEntry(*baseRule->dataSource);

			if ((scriptCurrent == scriptBase || !checkScript) && (currentRule->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER || currentRule->type == Model_Rule::PLAINTEXT)) {
				if (direction == 1) {
					rules.push_back(currentRule);
				} else {
					rules.push_front(currentRule);
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

int Controller_Helper_RuleMove::_countRulesUntilNextRealRule(Model_Rule* baseRule, int direction) {
	int result = 1;
	bool placeholderFound = false;
	Model_Rule* currentRule = baseRule;
	do {
		try {
			currentRule = &*this->grublistCfg->proxies.getNextVisibleRule(currentRule, direction);

			if (currentRule->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER || currentRule->type == Model_Rule::PLAINTEXT) {
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

std::list<Rule*> Controller_Helper_RuleMove::_removePlaceholdersFromSelection(std::list<Rule*> rules) {
	std::list<Rule*> result;
	for (std::list<Rule*>::iterator ruleIter = rules.begin(); ruleIter != rules.end(); ruleIter++) {
		Model_Rule* rule = &Model_Rule::fromPtr(*ruleIter);
		if (!(rule->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER || rule->type == Model_Rule::PLAINTEXT)) {
			result.push_back(rule);
		}
	}
	return result;
}
