#include "proxy.h"

Proxy::Proxy()
	: dataSource(NULL), permissions(0755), index(90)
{	
}

Proxy::Proxy(Script& dataSource)
	: dataSource(&dataSource), permissions(0755), index(90)
{
	rules.push_back(Rule(Rule::OTHER_ENTRIES_PLACEHOLDER, "*", true));
	sync(true, true);
}

void Proxy::importRuleString(const char* ruleString){
	rules.clear();

	bool inString = false, inAlias = false;
	std::string name;
	bool visible = false;
	for (const char* iter = ruleString; *iter; iter++) {
		if (!inString && *iter == '+')
			visible = true;
		else if (!inString && *iter == '-')
			visible = false;
		else if (*iter == '\'' && iter[1] != '\''){
			inString = !inString;
			if (!inString){
				if (inAlias)
					rules.back().outputName = name;
				else
					rules.push_back(Rule(Rule::NORMAL, name, visible));
				inAlias = false;
			}
			name = "";
		}
		else if (!inString && *iter == '*')
			rules.push_back(Rule(Rule::OTHER_ENTRIES_PLACEHOLDER, "*", visible));

		else if (inString){
			name += *iter;
			if (*iter == '\'')
				iter++;
		}
		else if (!inString && *iter == 'a' && *++iter == 's')
			inAlias = true;
	}
}
bool Proxy::sync(bool deleteInvalidRules, bool expand){
	if (this->dataSource){
		std::list<Rule> result;
		
		for (std::list<Rule>::iterator iter = this->rules.begin(); iter != this->rules.end(); iter++){
			if (iter->type != Rule::OTHER_ENTRIES_PLACEHOLDER){
				iter->dataSource = this->dataSource->getEntryByName(iter->__idname);
				if (iter->dataSource || !deleteInvalidRules){
					result.push_back(*iter);
				}
			}
			else
				result.push_back(*iter);
		}
		rules.swap(result);
		
		if (expand){
			std::list<Rule>::iterator other_entries_iter = this->rules.begin();
			while (other_entries_iter != this->rules.end() && other_entries_iter->type != Rule::OTHER_ENTRIES_PLACEHOLDER){
				//std::cout << other_entries_iter->outputName << " is no OTHER_ENTRIES_PLACEHOLDER" << std::endl;
				other_entries_iter++;
			}
			if (other_entries_iter == this->rules.end()){
				this->rules.push_front(Rule(Rule::OTHER_ENTRIES_PLACEHOLDER, "*", true));
				other_entries_iter = this->rules.begin();
			}
			std::list<Rule> otherEntryRules;
			for (Script::iterator iter = this->dataSource->begin(); iter != this->dataSource->end(); iter++){
				bool entryExists = false;
				for (std::list<Rule>::iterator rule_iter = this->rules.begin(); rule_iter != this->rules.end(); rule_iter++){
					if (&*iter == rule_iter->dataSource)
						entryExists = true;
				}
				if (!entryExists){
					otherEntryRules.push_back(Rule(*iter, other_entries_iter->isVisible)); //generate rule for given entry
				}
			}
			other_entries_iter++;
			this->rules.splice(other_entries_iter, otherEntryRules);
		}
		return true;
	}
	else
		return false;
}

bool Proxy::isModified() const {
	bool result = false;
	if (this->dataSource){
		if (this->rules.size()-1 == this->dataSource->size()){ //rules contains the other entries placeholder, so there is one more entry
			std::list<Rule>::const_iterator ruleIter = this->rules.begin();
			Script::iterator entryIter = this->dataSource->begin();
			if (ruleIter->type == Rule::OTHER_ENTRIES_PLACEHOLDER){ //the first element is the OTHER_ENTRIES_PLACEHOLDER by default.
				result = !ruleIter->isVisible; //If not visible, it's modified…
				ruleIter++;
			}
			while (!result && ruleIter != this->rules.end() && entryIter != this->dataSource->end()){
				if (ruleIter->outputName != entryIter->name || !ruleIter->isVisible)
					result = true;
			
				ruleIter++;
				entryIter++;
			}
		}
		else
			result = true;
	}
	return result;
}

bool Proxy::deleteFile(){
	int success = unlink(this->fileName.c_str());
	if (success == 0){
		this->fileName = "";
		return true;
	}
	else
		return false;
}

bool Proxy::generateFile(std::string const& path, int cfg_dir_prefix_length, std::string const& cfg_dir_noprefix){
	if (this->dataSource){
		FILE* proxyFile = fopen(path.c_str(), "w");
		if (proxyFile){
			this->fileName = path;
			fputs("#!/bin/sh\n#THIS IS A GRUB PROXY SCRIPT\n", proxyFile);
			fputs(("'"+this->dataSource->fileName.substr(cfg_dir_prefix_length)+"'").c_str(), proxyFile);
			fputs((" | "+cfg_dir_noprefix+"/bin/grubcfg_proxy \"").c_str(), proxyFile);
			int i = 0;
			for (std::list<Rule>::iterator ruleIter = this->rules.begin(); ruleIter != this->rules.end(); ruleIter++){
				fputs((std::string(*ruleIter)+"\n").c_str(), proxyFile); //write rule
			}
			fputs("\"", proxyFile);
			fclose(proxyFile);
			chmod(path.c_str(), this->permissions);
			return true;
		}
	}
	return false;
}

bool Proxy::isExecutable() const {
	return this->permissions & 0111;
}


void Proxy::set_isExecutable(bool value){
	if (value)
		permissions |= 0111;
	else
		permissions &= ~0111;
}

std::string Proxy::getScriptName(){
	if (this->dataSource)
		return this->dataSource->name;
	else
		return "?";
}

