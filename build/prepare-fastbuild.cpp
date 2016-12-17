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

#include "common-build.hpp"
#include <memory>
#include <string>
#include <iostream>
#include <list>

namespace GcBuild
{
	std::string makeReadable(std::string str)
	{
		str = replaceStringContents("\n", "⏷", str);
		str = replaceStringContents("\t", "⏵", str);
		return str;
	}

	std::string renderString(std::string const& str)
	{
		std::string rendered;
		if (str.size() < 20) {
			rendered = makeReadable(str);
		} else {
			rendered = makeReadable(str.substr(0, 8)) + " ... " + makeReadable(str.substr(str.size() - 7));
		}
		return rendered;
	}

	class AbstractContent
	{
		public: std::list<std::shared_ptr<AbstractContent>> children;

		public: virtual ~AbstractContent(){}

		public: void dumpTree(unsigned int indent = 0)
		{
			unsigned int pos = 0;
			for (auto& child : this->children) {
				std::cout << GcBuild::stringRepeat("  ", indent) << "(" << ++pos << ") " << child->describe() << std::endl;
				child->dumpTree(indent + 1);
			}
		}

		public: virtual std::string describe() = 0;

		public: virtual bool isContainer()
		{
			return false;
		}

		public: virtual bool isEndOfContainer()
		{
			return false;
		}
	};

	class Namespace : public AbstractContent
	{
		public: std::string name;
		public: virtual ~Namespace(){}
		public: std::string describe()
		{
			return "namespace " + this->name;
		}
		public: virtual bool isContainer()
		{
			return true;
		}
	};

	class Class : public AbstractContent
	{
		public: std::string name;
		public: std::string properties;
		public: virtual ~Class(){}
		public: std::string describe()
		{
			std::string desc = "class " + this->name;
			if (this->properties.size()) {
				desc += " | Properties: " + renderString(this->properties);
			}
			return desc;
		}
		public: virtual bool isContainer()
		{
			return true;
		}
	};

	/**
	 * generic container with brackets
	 */
	class Bracket : public AbstractContent
	{
		public: std::string openingBracket;
		public: std::string closingBracket;
		public: virtual ~Bracket(){}
		public: Bracket(std::string const& bracket) : openingBracket(bracket) {
			if (bracket.size() == 0) {
				throw std::runtime_error("got zero sized bracket");
			}
			switch (bracket[0]) {
				case '{': this->closingBracket = '}'; break;
				case '(': this->closingBracket = ')'; break;
				case '[': this->closingBracket = ']'; break;
			}
		}
		public: std::string describe()
		{
			return "bracket: " + this->openingBracket + " ... " + this->closingBracket;
		}
		public: virtual bool isContainer()
		{
			return true;
		}
	};

	/**
	 * dummy object to notify about the end of a bracket
	 */
	class ClosingBracket : public AbstractContent
	{
		public: std::string bracket;
		public: virtual ~ClosingBracket(){}
		public: ClosingBracket(std::string const& bracket) : bracket(bracket) {}
		public: std::string describe()
		{
			return "Closing bracket: " + this->bracket;
		}
		public: virtual bool isEndOfContainer()
		{
			return true;
		}
	};

	class GenericCode : public AbstractContent
	{
		public: std::string data;
		public: std::string type;

		public: GenericCode(std::string const& type, std::string const& data)
			: type(type), data(data)
		{}

		public: virtual ~GenericCode(){}

		public: virtual std::string describe()
		{
			std::string desc = this->type + " [" + GcBuild::intToString(this->data.size()) + "]";

			desc += " : " + renderString(this->data);

			return desc;
		}

		private:
	};

	class File : public AbstractContent
	{
		public: virtual ~File(){}
		public: virtual std::string describe()
		{
			return "file";
		}
	};
	
	class Parser
	{
		public: std::string content;
		private: size_t pos = 0;

		public: Parser(std::string const& content = "")
			: content(content)
		{}

		public: std::shared_ptr<File> parse()
		{
			this->pos = 0;

			std::shared_ptr<File> result = std::make_shared<File>();

			std::list<std::shared_ptr<AbstractContent>> containerPath;
			containerPath.push_back(result);

			std::shared_ptr<AbstractContent> nextPart = nullptr;
			while ((nextPart = this->readNextPart())) {
				if (dynamic_cast<GenericCode*>(nextPart.get())
					&& dynamic_cast<GenericCode*>(nextPart.get())->type == "char"
					&& containerPath.back()->children.size()
					&& dynamic_cast<GenericCode*>(containerPath.back()->children.back().get())
					&& dynamic_cast<GenericCode*>(containerPath.back()->children.back().get())->type == "char") {
					// merge chars
					dynamic_cast<GenericCode&>(*containerPath.back()->children.back()).data += dynamic_cast<GenericCode&>(*nextPart).data;
				} else if (!nextPart->isEndOfContainer()) {
					containerPath.back()->children.push_back(nextPart);
				}
				if (nextPart->isContainer()) {
					containerPath.push_back(nextPart);
				}
				if (nextPart->isEndOfContainer()) {
					containerPath.pop_back();
				}
			}

			return result;
		}

		private: std::shared_ptr<AbstractContent> readNextPart()
		{
			std::shared_ptr<AbstractContent> result = nullptr;
			(result = this->readComment()) ||
			(result = this->readString()) ||
			(result = this->readOpeningBracket()) ||
			(result = this->readClosingBracket()) ||
			(result = this->readPreprocessor()) ||
			(result = this->readNamespace()) ||
			(result = this->readClass()) ||
			(result = this->readChar());

			return result;
		}

		private: std::shared_ptr<GenericCode> readComment()
		{
			std::shared_ptr<GenericCode> result = nullptr;

			if (this->content.substr(this->pos, 2) == "/*") {
				size_t end = this->content.find("*/", this->pos + 2) + 2;
				result = std::make_shared<GenericCode>("multiline-comment", this->content.substr(this->pos, end - this->pos));
				this->pos = end;
			} else if (this->content.substr(this->pos, 2) == "//") {
				size_t end = this->content.find_first_of('\n', this->pos + 2) + 1;
				result = std::make_shared<GenericCode>("singleline-comment", this->content.substr(this->pos, end - this->pos));
				this->pos = end;
			}

			return result;
		}

		private: std::shared_ptr<GenericCode> readString()
		{
			std::shared_ptr<GenericCode> result = nullptr;

			std::string nextChar = this->content.substr(this->pos, 1);
			if (nextChar == "'" || nextChar == "\"") {
				result = std::make_shared<GenericCode>("string", nextChar);
				this->pos++;
				bool goOn = false;
				do {
					goOn = false;
					size_t end = this->content.find_first_of(nextChar + "\\", this->pos);
					if (end == -1) {
						throw std::runtime_error("found unterminated string");
					}
					if (this->content[end] == '\\') {
						// if we found an escape char, read it and the following char and coninue this loop...
						result->data += this->content.substr(this->pos, end - this->pos + 2);
						this->pos = end + 2;
						goOn = true;
					} else {
						//... otherwise end here
						result->data += this->content.substr(this->pos, end - this->pos + 1);
						this->pos = end + 1;
					}
				} while (goOn);
			}

			return result;
		}

		private: std::shared_ptr<GenericCode> readPreprocessor()
		{
			std::shared_ptr<GenericCode> result = nullptr;

			if (this->content.substr(this->pos, 1) == "#") {
				size_t end = this->content.find_first_of('\n', this->pos + 2) + 1;
				result = std::make_shared<GenericCode>("preprocessor", this->content.substr(this->pos, end - this->pos));
				this->pos = end;
			}

			return result;
		}

		private: std::shared_ptr<Namespace> readNamespace()
		{
			std::shared_ptr<Namespace> result = nullptr;

			if (this->content.substr(this->pos, std::string("namespace").size()) == "namespace") {
				pos += std::string("namespace").size();
				result = std::make_shared<Namespace>();
				size_t contentBracketPos = this->content.find_first_of('{', this->pos);
				result->name = GcBuild::trim(this->content.substr(this->pos, contentBracketPos - this->pos - 1));
				this->pos = contentBracketPos + 1;
			}

			return result;
		}

		private: std::shared_ptr<Class> readClass()
		{
			std::shared_ptr<Class> result = nullptr;

			if (this->content.substr(this->pos, std::string("class").size()) == "class") {
				pos += std::string("class").size();
				result = std::make_shared<Class>();
				size_t contentBracketPos = this->content.find_first_of('{', this->pos);
				std::string nameAndProperties = this->content.substr(this->pos, contentBracketPos - this->pos - 1);
				size_t colonPos = nameAndProperties.find_first_of(':');
				if (colonPos == -1) { // no properties set
					result->name = nameAndProperties;
				} else {
					result->name = GcBuild::trim(nameAndProperties.substr(0, colonPos));
					result->properties = GcBuild::trim(nameAndProperties.substr(colonPos + 1));
				}
				this->pos = contentBracketPos + 1;
			}

			return result;
		}

		private: std::shared_ptr<Bracket> readOpeningBracket()
		{
			std::shared_ptr<Bracket> result = nullptr;

			std::string bracket = this->content.substr(this->pos, 1);
			if (bracket == "{" || bracket == "(" || bracket == "[") {
				result = std::make_shared<Bracket>(bracket);
				pos++;
			}

			return result;
		}

		private: std::shared_ptr<ClosingBracket> readClosingBracket()
		{
			std::shared_ptr<ClosingBracket> result = nullptr;

			std::string bracket = this->content.substr(this->pos, 1);
			if (bracket == "}" || bracket == ")" || bracket == "]") {
				result = std::make_shared<ClosingBracket>(bracket);
				pos++;
			}

			return result;
		}

		/**
		 * just read a char - should be used as fallback
		 */
		private: std::shared_ptr<GenericCode> readChar()
		{
			std::string data = this->content.substr(this->pos, 1);
			if (data == "") {
				return nullptr;
			}
			this->pos++;
			return std::make_shared<GenericCode>("char", data);
		}
	};

	class Optimizer
	{
		private: std::string root;
		public: Optimizer(std::string const& root)
			: root(root)
		{}

		public: void optimize(
			std::string const& inputFile,
			std::string const& headerDest,
			std::string const& implDest
		) {
			auto content = GcBuild::getFileContents(this->root + "/" + inputFile);

			auto file = std::make_shared<Parser>(content)->parse();

			file->dumpTree();
		}
	};
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "I need the root path of the project as argument #1" << std::endl;
		return 1;
	}

	auto optimizer = std::make_shared<GcBuild::Optimizer>(argv[1]);

	optimizer->optimize(
		"src/Controller/AboutController.hpp",
		"src/Controller/AboutController.h",
		"src/Controller/AboutController.cpp"
	);
}
