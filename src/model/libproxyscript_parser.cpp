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

#include "libproxyscript_parser.h"

ProxyScriptData::ProxyScriptData(FILE* fpProxyScript)
	: is_valid(false)
{
	load(fpProxyScript);
}

bool ProxyScriptData::is_proxyscript(FILE* proxy_fp){
	int c;
	//skip first line
	while ((c = fgetc(proxy_fp)) != EOF){
		if (c == '\n')
			break;
	}
	//compare the line start
	std::string textBefore = "#THIS IS A GRUB PROXY SCRIPT";
	bool match_error = false;
	for (int i = 0; i < textBefore.length() && (c = fgetc(proxy_fp)) != EOF; i++){
		if (c != textBefore[i]){
			match_error = true; //It's not a proxy.
			break;
		}
	}
	
	//go to the next line
	if (!match_error){
		//read the script name (ends by line break)
		while ((c = fgetc(proxy_fp)) != EOF){
			if (c == '\n')
				break;
		}
	}
	return !match_error;
}

bool ProxyScriptData::is_proxyscript(std::string const& filePath){
	bool result = false;
	FILE* f = fopen(filePath.c_str(), "r");
	if (f){
		result = ProxyScriptData::is_proxyscript(f);
		fclose(f);
	}
	return result;
}

bool ProxyScriptData::load(FILE* fpProxyScript){
	//THIS ALGORITHM IS ONLY USEFUL FOR GENERATED PROXIES
	this->scriptCmd = "";
	this->proxyCmd = "";
	this->ruleString = "";
	
	if (ProxyScriptData::is_proxyscript(fpProxyScript)){
		int c;
		bool is_begin_of_row = true, is_comment = false;
		int parseStep = 0;
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
					if (this->scriptCmd.length() == 0 && inQuotes == false && c == '\''){
						inQuotes = true;
					}
					else if (!inQuotes && c != ' ' || inQuotes && c != '\''){
						this->scriptCmd += char(c);
					}
					else {
						inQuotes = false;
						parseStep = 1;
					}
				}
				else if (parseStep == 1 && c != ' ' && c != '|'){
					parseStep = 2;
				}
				if (parseStep == 2){
					if (c != ' ')
						this->proxyCmd += char(c);
					else
						parseStep = 3;
				}
				if (parseStep == 3){
					if (c == '"' && !inQuotes)
						inQuotes = true;
					else if ((c == '"' || c == '\\') && inQuotes && (this->ruleString.length() > 0 && this->ruleString[this->ruleString.length()-1] == '\\'))
						this->ruleString[this->ruleString.length()-1] = char(c);
					else if (c == '"' && inQuotes){
						if (this->scriptCmd != "" && this->proxyCmd != "" && this->ruleString != ""){
							success = true;
						}
						else
							parseStep = 0;
					}
					else {
						this->ruleString += char(c);
					}
				}
				is_begin_of_row = false;
			}
		
			if (c == '\n')
				is_begin_of_row = true;
		}
		this->is_valid = true;
	}
}

ProxyScriptData::operator bool(){
	return is_valid;
}
