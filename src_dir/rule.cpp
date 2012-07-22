#include "rule.h"

Rule::Rule(Entry& source, bool isVisible) //generate rule for given entry
	: type(Rule::NORMAL), isVisible(isVisible), outputName(source.name), dataSource(&source)
{}

Rule::operator std::string(){
	std::string result = isVisible ? "+" : "-";
	if (dataSource)
		result += "'"+dataSource->name+"'"; //TODO: replace containing "'" by "''"
	else if (type == OTHER_ENTRIES_PLACEHOLDER)
		result += "*";
	else
		result += "???";
	if (type == NORMAL && (dataSource && dataSource->name != outputName))
		result += " as '"+outputName+"'";
	return result;
}

Rule::Rule(RuleType type, std::string name, bool isVisible)
	: type(type), isVisible(isVisible), __idname(name), outputName(name), dataSource(NULL)
{}

std::string Rule::getScriptName() const {
	if (this->dataSource)
		return this->dataSource->name;
	else
		return "?";
}


void Rule::print() const {
	if (this->isVisible && this->dataSource){
		std::cout << "menuentry \""+this->outputName+"\""+this->dataSource->extension+"{\n"+this->dataSource->content+"}\n";
	}
}
