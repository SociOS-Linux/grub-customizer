#include "libproxyscript_parser.h"

ProxyscriptData parseProxyScript(FILE* fpProxyScript){
	//THIS ALGORITHM IS ONLY USEFUL FOR GENERATED PROXIES
	int c;
	bool is_begin_of_row = true, is_comment = false;
	int parseStep = 0;
	ProxyscriptData data;
	bool inQuotes = false;
	bool success = false;
	while ((c = fgetc(fpProxyScript)) != EOF && !success){
		if (is_begin_of_row && c == '#'){
			is_comment = true;
			is_begin_of_row = false;
		}
		else if (is_comment && c == '\n'){
			is_comment = false;
		}
		else if (!is_comment) { //the following code will only parse shell commands (comments are filtered out!)
			if (parseStep == 0){
				if (c != ' ')
					data.scriptCmd += char(c);
				else
					parseStep = 1;
			}
			if (parseStep == 1 && c != ' ' && c != '|'){
				parseStep = 2;
			}
			if (parseStep == 2){
				if (c != ' ')
					data.proxyCmd += char(c);
				else
					parseStep = 3;
			}
			if (parseStep == 3){
				if (c == '"' && !inQuotes)
					inQuotes = true;
				else if ((c == '"' || c == '\\') && inQuotes && (data.ruleString.length() > 0 && data.ruleString[data.ruleString.length()-1] == '\\'))
					data.ruleString[data.ruleString.length()-1] = char(c);
				else if (c == '"' && inQuotes){
					if (data.scriptCmd != "" && data.proxyCmd != "" && data.ruleString != ""){
						success = true;
					}
					else
						parseStep = 0;
				}
				else {
					data.ruleString += char(c);
				}
			}
		}
		
		if (c == '\n')
			is_begin_of_row = true;
	}
	return data;
}
