/*
 * Copyright (C) 2010-2014 Daniel Richter <danielrichter2007@web.de>
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
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <map>
#include <list>
#include <algorithm>

class FileConverter
{
	private: std::shared_ptr<std::ifstream> inputFile;
	private: std::shared_ptr<std::ofstream> outputHeader;
	private: std::shared_ptr<std::ofstream> outputSource;
	private: std::string outputHeaderPath;

	private: char nextChr = 0; // to be used by readNext
	private: bool hasNextChr = false; // to be used by readNext

	public: FileConverter(
		std::shared_ptr<std::ifstream> inputFile,
		std::shared_ptr<std::ofstream> outputHeader,
		std::shared_ptr<std::ofstream> outputSource,
		std::string outputHeaderPath
	) :
		inputFile(inputFile),
		outputHeader(outputHeader),
		outputSource(outputSource),
		outputHeaderPath(outputHeaderPath)
	{
	}

	public: void exec()
	{
		*this->outputSource << "#include \"" << outputHeaderPath << "\"" << std::endl;

		while (!inputFile->eof()) {
			std::string token = this->readToken();
			if (token == "class" || token == "struct") {
				this->processClass(token, "");
			} else if (token == "template") {
				this->processTemplate(token);
			} else {
				*this->outputHeader << token;
			}
		}
	}

	private: void processClass(std::string const& keyword, std::string const& parentClass)
	{
		*this->outputHeader << keyword;

		std::string className;

		if (parentClass != "") {
			className = parentClass + "::";
		}

		// read class name
		while (!inputFile->eof()) {
			std::string token = this->readToken();
			*this->outputHeader << token;

			if (this->isTokenChar(token[0])) {
				className = token;
				break;
			}
		}

		// read until begin of class
		*this->outputHeader << this->readUntilToken("{");

		// read class content
		while (!inputFile->eof()) {
			std::string token = this->readToken();
			if (token == "class" || token == "struct") {
				this->processClass(token, className);
			} else if (token == "private" || token == "protected" || token == "public") {
				*this->outputHeader << token;
				*this->outputHeader << this->readUntilToken(":");
			} else if (!this->isTokenChar(token[0])) {
				*this->outputHeader << token;
			} else {
				std::string memberDeclaration = token + this->readUntilTokens({"(", ";", "="});
				char lastChar = memberDeclaration[memberDeclaration.length() - 1];
				if (lastChar == '(') { // is function
					this->processFunction(memberDeclaration, className);
				} else if (lastChar == ';') { // is property
					*this->outputHeader << memberDeclaration;
				} else { // is incomplete property
					*this->outputHeader << memberDeclaration << this->readUntilToken(";");
				}
			}
		}
	}

	private: void processFunction(std::string memberDeclaration, std::string const& className)
	{
		unsigned int bracketPos = memberDeclaration.length() - 1;

		memberDeclaration += this->readUntilClosingBracket("(");

		memberDeclaration += this->readUntilTokens({"{", ":"});

		std::string memberDefinition = memberDeclaration;

		memberDeclaration = memberDeclaration.substr(0, memberDeclaration.length() - 1);

		// remove override keyword from definition
		if (memberDefinition.find("override") != -1) {
			int pos = memberDefinition.find("override");
			memberDefinition.replace(pos, 8, "");
		}

		// inject class name into function name (definition)
		{
			int functionNameEnd = memberDefinition.find_last_not_of(" \t\n", bracketPos);
			int functionNameBegin = memberDefinition.find_last_of(" \t\n", functionNameEnd);
			functionNameBegin++;
			memberDefinition.replace(functionNameBegin, 0, className + "::");
		}

		// remove default values from definition
		while (memberDefinition.find_first_of('=') != -1) {
			int assignmentPos = memberDefinition.find_first_of('=');
			int endPos = memberDefinition.find_first_of(",)", assignmentPos);
			memberDefinition.replace(assignmentPos, endPos - assignmentPos, "");
		}

		// expend definition by initializer list
		if (memberDefinition[memberDefinition.size() - 1] == ':') {
			memberDefinition += this->readUntilToken("{");
		}

		memberDefinition += this->readUntilClosingBracket("{");
		*this->outputHeader << memberDeclaration << ";";
		*this->outputSource << memberDefinition << "\n";

	}

	private: void processTemplate(std::string const& keyword)
	{
		*this->outputHeader << keyword;
		*this->outputHeader << this->readUntilToken("{");
		*this->outputHeader << this->readUntilClosingBracket("{");
	}

	private: std::string readUntilClosingBracket(std::string const& openingBracket)
	{
		std::string result;

		std::stack<std::string> innerBrackets;
		innerBrackets.push(openingBracket);
		auto bracketMap = this->getBracketMap();

		if (bracketMap.find(openingBracket) == bracketMap.end()) {
			throw std::logic_error("invalid bracket given");
		}

		while (!inputFile->eof()) {
			std::string token = this->readToken();

			result += token;

			if (bracketMap.find(token) != bracketMap.end()) {
				innerBrackets.push(token);
			} else if (token == bracketMap[innerBrackets.top()]) {
				innerBrackets.pop();
				if (innerBrackets.size() == 0) {
					break;
				}
			}
		}

		return result;
	}

	private: std::string readUntilToken(std::string const& searchToken)
	{
		return this->readUntilTokens({searchToken});
	}

	private: std::string readUntilTokens(std::list<std::string> const& searchTokens)
	{
		std::string result;

		while (!inputFile->eof()) {
			std::string token = this->readToken();

			result += token;

			if (std::find(searchTokens.begin(), searchTokens.end(), token) != searchTokens.end()) {
				break; // class begin found - start parsing the class content
			}
		}

		return result;
	}

	private: std::string readToken()
	{
		std::string result;

		bool readNext = false;

		do {
			readNext = false;

			char c = this->readChar();
			result += c;

			if (c == '"' || c == '\'') {
				result += this->readUntilChar(c);
			} else if (c == '/' && this->nextChr == '*') {
				result += this->readUntilEndOfMultilineComment();
			} else if (c == '/' && this->nextChr == '/') {
				result += this->readUntilChar('\n');
			} else if (this->isTokenChar(c) && this->isTokenChar(this->nextChr)) {
				readNext = true;
			}
		} while (readNext && !this->inputFile->eof());

		return result;
	}

	private: std::string readUntilChar(char const& untilChar)
	{
		std::string result;

		if (this->nextChr == untilChar) {
			result += this->readChar(); // read last quote and return
			return result;
		}

		bool readNext = false;

		do {
			readNext = false;
			char c = this->readChar();
			result += c;

			if (this->nextChr == untilChar && c != '\\') {
				result += this->readChar(); // read last quote and return
			} else {
				readNext = true;
			}
		} while (readNext && !this->inputFile->eof());

		return result;
	}

	private: std::string readUntilEndOfMultilineComment()
	{
		std::string result;

		bool readNext = false;

		do {
			readNext = false;
			char c = this->readChar();
			result += c;

			if (c == '*' && this->nextChr == '/') {
				result += this->readChar(); // read last quote and return
			} else {
				readNext = true;
			}
		} while (readNext && !this->inputFile->eof());

		return result;
	}

	private: char readChar()
	{
		if (!this->hasNextChr) {
			this->inputFile->get(this->nextChr);
			this->hasNextChr = true;
		}
		char c = this->nextChr;
		this->inputFile->get(this->nextChr);
		return c;
	}

	private: bool isTokenChar(char const& c)
	{
		return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '#' || c == '_';
	}

	private: std::map<std::string, std::string> getBracketMap()
	{
		return {
			{"{", "}"},
			{"[", "]"},
			{"(", ")"}
			//{"<", ">"} not yet supported
		};
	}
};

int main(int argc, char** argv) {
	if (argc != 4) {
		std::cerr << "missing arguments. Syntax: header_extract INPUT.hpp OUTPUT.hpp OUTPUT.cpp" << std::endl;
		return 1;
	}
	std::string inputFile = argv[1];
	std::string outputHeader = argv[2];
	std::string outputSource = argv[3];

	FileConverter converter(
		std::make_shared<std::ifstream>(inputFile),
		std::make_shared<std::ofstream>(outputHeader),
		std::make_shared<std::ofstream>(outputSource),
		outputHeader
	);

	converter.exec();
}
