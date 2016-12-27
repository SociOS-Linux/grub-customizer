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
#include <map>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>

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

	std::string getNextWord(std::string const& str, size_t pos)
	{
		static const std::string wordChars = buildCharString('a', 'z') + buildCharString('A', 'Z') + buildCharString('0', '9') + "_";
		std::string result;

		std::string firstChar = str.substr(pos, 1);

		if (firstChar.find_first_of(wordChars) != -1) {
			size_t end = str.find_first_not_of(wordChars, pos);
			result = str.substr(pos, end - pos);
		}

		return result;
	}

	class AbstractContent : public std::enable_shared_from_this<AbstractContent>
	{
		public: enum class RenderDest
		{
			HEADER,
			SOURCE
		};

		public: std::list<std::shared_ptr<AbstractContent>> children;
		public: bool isGrouped = false;

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

		public: virtual void optimize()
		{
			for (auto& child : this->children) {
				child->optimize();
			}
		}

		public: virtual std::string render(std::list<std::string> path)
		{
			std::string result;
			for (auto& child : this->children) {
				result += child->render(path);
			}
			return result;
		}

		public: virtual std::string getPathPart()
		{
			return "";
		}

		public: virtual bool isFunction()
		{
			return false;
		}

		public: std::map<std::shared_ptr<AbstractContent>, std::list<std::string>> findFunctions(
			std::list<std::string> currentPath = {}
		) {
			if (this->getPathPart() != "") {
				currentPath.push_back(this->getPathPart());
			}
			std::map<std::shared_ptr<AbstractContent>, std::list<std::string>> result;
			for (auto& child : this->children) {
				if (child->isFunction()) {
					result[child] = currentPath;
				}
				auto childItems = child->findFunctions(currentPath);
				result.insert(childItems.begin(), childItems.end());
			}
			return result;
		}

		public: virtual void groupChars()
		{
			std::list<std::shared_ptr<AbstractContent>> newList;
			for (auto& child : this->children) {
				child->groupChars(); // recursion

				if (newList.size()) {
					try {
						newList.back()->mergeIfType("char", child);
					} catch (std::runtime_error const& e) {
						newList.push_back(child);
					}
				} else {
					newList.push_back(child);
				}
			}
			this->children = newList;
		}

		public: virtual void groupContainers()
		{
			std::list<std::shared_ptr<AbstractContent>> containerPath;
			containerPath.push_back(this->shared_from_this());

			auto childrenOriginal = this->children;
			this->children.clear();
			for (auto& child : childrenOriginal) {
				if (child->isGrouped) {
					// if already grouped, run through children to find ungrouped containers
					containerPath.back()->children.push_back(child);
					child->groupContainers();
				} else {
					// it's still a flat container
					if (!child->isEndOfContainer()) {
						containerPath.back()->children.push_back(child);
					}
					if (child->isContainer()) {
						containerPath.push_back(child);
					}
					if (child->isEndOfContainer()) {
						containerPath.back()->isGrouped = true;
						containerPath.pop_back();
					}
				}
			}
		}

		public: virtual void mergeIfType(std::string const& type, std::shared_ptr<AbstractContent> other)
		{
			throw std::runtime_error("merge not supported");
		}

		public: virtual std::string getWord()
		{
			return "";
		}

		public: virtual bool isWhitespace()
		{
			return false;
		}

		public: virtual bool isComment()
		{
			return false;
		}
	};

	class Property : public AbstractContent
	{
		public: virtual ~Property(){}
		public: std::string describe()
		{
			return "property";
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

		public: virtual std::string render(std::list<std::string> path) override
		{
			return this->openingBracket + AbstractContent::render(path) + this->closingBracket;
		}

		public: static std::shared_ptr<Bracket> readFromString(std::string const& content, size_t& pos)
		{
			std::shared_ptr<Bracket> result = nullptr;

			std::string bracket = content.substr(pos, 1);
			if (bracket == "{" || bracket == "(" || bracket == "[") {
				result = std::make_shared<Bracket>(bracket);
				pos++;
			}

			return result;
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

		public: static std::shared_ptr<ClosingBracket> readFromString(std::string const& content, size_t& pos)
		{
			std::shared_ptr<ClosingBracket> result = nullptr;

			std::string bracket = content.substr(pos, 1);
			if (bracket == "}" || bracket == ")" || bracket == "]") {
				result = std::make_shared<ClosingBracket>(bracket);
				pos++;
			}

			return result;
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

		public: virtual std::string render(std::list<std::string> path) override
		{
			return this->data;
		}

		public: virtual void mergeIfType(std::string const& type, std::shared_ptr<AbstractContent> other)
		{
			if (this->type != type) {
				throw std::runtime_error("wrong type");
			}
			auto otherAsGenericCode = std::dynamic_pointer_cast<GenericCode>(other);
			if (otherAsGenericCode == nullptr) {
				throw std::runtime_error("other object is not generic code");
			}
			if (otherAsGenericCode->type != type) {
				throw std::runtime_error("other object is wrong type");
			}
			this->data += otherAsGenericCode->data;
		}

		public: virtual std::string getWord() override
		{
			if (this->type != "word") {
				return "";
			}
			return this->data;
		}

		public: virtual bool isWhitespace() override
		{
			return this->data.find_first_not_of(" \n\r\t") == -1;
		}

		public: virtual bool isComment() override
		{
			return this->type == "multiline-comment" || this->type == "singleline-comment";
		}

		public: static std::shared_ptr<GenericCode> readCommentFromString(std::string const& content, size_t& pos)
		{
			std::shared_ptr<GenericCode> result = nullptr;

			if (content.substr(pos, 2) == "/*") {
				size_t end = content.find("*/", pos + 2) + 2;
				result = std::make_shared<GenericCode>("multiline-comment", content.substr(pos, end - pos));
				pos = end;
			} else if (content.substr(pos, 2) == "//") {
				size_t end = content.find_first_of('\n', pos + 2) + 1;
				result = std::make_shared<GenericCode>("singleline-comment", content.substr(pos, end - pos));
				pos = end;
			}

			return result;
		}

		public: static std::shared_ptr<GenericCode> readStringFromString(std::string const& content, size_t& pos)
		{
			std::shared_ptr<GenericCode> result = nullptr;

			std::string nextChar = content.substr(pos, 1);
			if (nextChar == "'" || nextChar == "\"") {
				result = std::make_shared<GenericCode>("string", nextChar);
				pos++;
				bool goOn = false;
				do {
					goOn = false;
					size_t end = content.find_first_of(nextChar + "\\", pos);
					if (end == -1) {
						throw std::runtime_error("found unterminated string");
					}
					if (content[end] == '\\') {
						// if we found an escape char, read it and the following char and coninue this loop...
						result->data += content.substr(pos, end - pos + 2);
						pos = end + 2;
						goOn = true;
					} else {
						//... otherwise end here
						result->data += content.substr(pos, end - pos + 1);
						pos = end + 1;
					}
				} while (goOn);
			}

			return result;
		}

		public: static std::shared_ptr<GenericCode> readPreprocessorFromString(std::string const& content, size_t& pos)
		{

			std::shared_ptr<GenericCode> result = nullptr;

			if (content.substr(pos, 1) == "#") {
				size_t end = content.find_first_of('\n', pos + 2) + 1;
				result = std::make_shared<GenericCode>("preprocessor", content.substr(pos, end - pos));
				pos = end;
			}

			return result;
		}



		public: static std::shared_ptr<GenericCode> readAccessControlFromString(std::string const& content, size_t& pos)
		{
			std::shared_ptr<GenericCode> accessControl = nullptr;

			std::string wordStr = getNextWord(content, pos);
			if (wordStr == "private" || wordStr == "protected" || wordStr == "public") {
				pos += wordStr.size();
				size_t end = content.find_first_of(':', pos);
				accessControl = std::make_shared<GenericCode>("accessControl", wordStr + content.substr(pos, end - pos + 1));
				pos = end  + 1;
			}

			return accessControl;
		}

		public: static std::shared_ptr<GenericCode> readWordFromString(std::string const& content, size_t& pos)
		{
			std::shared_ptr<GenericCode> word = nullptr;

			std::string wordStr = getNextWord(content, pos);
			if (wordStr != "") {
				word = std::make_shared<GenericCode>("word", wordStr);
				pos += wordStr.size();
			}

			return word;
		}

		/**
		 * just read a char - should be used as fallback
		 */
		public: static std::shared_ptr<GenericCode> readCommandSeparatorFromString(std::string const& content, size_t& pos)
		{
			std::string data = content.substr(pos, 1);
			if (data != ";") {
				return nullptr;
			}
			pos++;
			return std::make_shared<GenericCode>("commandSeparator", data);
		}

		/**
		 * just read a char - should be used as fallback
		 */
		public: static std::shared_ptr<GenericCode> readCharFromString(std::string const& content, size_t& pos)
		{
			std::string data = content.substr(pos, 1);
			if (data == "") {
				return nullptr;
			}
			pos++;
			return std::make_shared<GenericCode>("char", data);
		}
	};

	class Function : public AbstractContent
	{
		public: std::list<std::shared_ptr<AbstractContent>> beforeAccess;
		public: std::shared_ptr<GenericCode> access;
		public: std::list<std::shared_ptr<AbstractContent>> returnValue;
		public: std::string name;
		public: std::list<std::shared_ptr<AbstractContent>> intermediateStuff; // between ) and {
		public: std::shared_ptr<Bracket> parameterList;
		public: virtual ~Function(){}
		public: std::string describe()
		{
			return "function " + this->name;
		}
		public: virtual bool isContainer()
		{
			return true;
		}

		public: virtual std::string render(RenderDest dest, std::list<std::string> path)
		{
			std::string content;
			if (dest == AbstractContent::RenderDest::HEADER) {
				for (auto& before : this->beforeAccess) {
					content += before->render(path);
				}
			}

			if (this->access && dest == AbstractContent::RenderDest::HEADER) {
				content += this->access->render(path);
			}
			for (auto& retValPart : this->returnValue) {
				content += retValPart->render(path);
			}
			if (dest == AbstractContent::RenderDest::SOURCE) {
				content += join(path, "::") + "::";
			}
			content += this->name;
			content += this->parameterList->render(path);
			if (this->name != path.back() || dest == RenderDest::SOURCE) {
				for (auto& interPart : this->intermediateStuff) {
					// source must not have the keyword "override"
					if (dest == RenderDest::SOURCE && this->isWord(interPart, "override")) {
						continue;
					}
					content += interPart->render(path);
				}
			}
			if (dest == AbstractContent::RenderDest::SOURCE) {
				content += "{" + AbstractContent::render(path) + "}";
			} else {
				content += ";";
			}
			return content;
		}

		private: bool isWord(std::shared_ptr<AbstractContent> obj, std::string word)
		{
			auto objectAsGenericCode = std::dynamic_pointer_cast<GenericCode>(obj);
			if (objectAsGenericCode == nullptr) {
				return false;
			}
			if (objectAsGenericCode->type != "word") {
				return false;
			}
			if (objectAsGenericCode->data != word) {
				return false;
			}
			return true;
		}

		public: virtual std::string render(std::list<std::string> path) override
		{
			return this->render(AbstractContent::RenderDest::HEADER, path);
		}

		public: virtual bool isFunction()
		{
			return true;
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

		public: virtual std::string render(std::list<std::string> path) override
		{
			path.push_back(this->getPathPart());
			return "namespace " + this->name + " {" + AbstractContent::render(path) + "}";
		}

		public: virtual std::string getPathPart() override
		{
			return this->name;
		}

		public: static void transformNamespaces(std::shared_ptr<AbstractContent> base)
		{
			auto childrenOriginal = base->children;
			base->children.clear();

			auto iter = childrenOriginal.begin();

			while (iter != childrenOriginal.end()) {
				auto loopStartIter = iter;

				// find word "namespace"
				iter = std::find_if(iter, childrenOriginal.end(), [] (std::shared_ptr<AbstractContent> item) {
					return item->getWord() == "namespace";
				});

				// add everything in front of the word "namespace" to new list
				if (iter != loopStartIter) {
					base->children.insert(base->children.end(), loopStartIter, iter);
				}

				if (iter == childrenOriginal.end()) {
					break;
				}

				auto namespaceWordIter = iter++;

				// find namespace name
				iter = std::find_if(iter, childrenOriginal.end(), &Namespace::isNonWhitespace);

				if (iter == childrenOriginal.end()) {
					base->children.insert(base->children.end(), loopStartIter, iter);
					break;
				}

				auto namespaceNameIter = iter++;

				if (!std::dynamic_pointer_cast<GenericCode>(*namespaceNameIter) || std::dynamic_pointer_cast<GenericCode>(*namespaceNameIter)->type != "word") {
					base->children.insert(base->children.end(), loopStartIter, std::prev(iter, 1));
					continue; // after word "namespace" we found no namespace name
				}

				// find opening bracket
				iter = std::find_if(iter, childrenOriginal.end(), &Namespace::isNonWhitespace);

				if (iter == childrenOriginal.end()) {
					base->children.insert(base->children.end(), loopStartIter, iter);
					break;
				}

				auto bracketIter = iter++;

				if (!std::dynamic_pointer_cast<Bracket>(*bracketIter) || std::dynamic_pointer_cast<Bracket>(*bracketIter)->openingBracket != "{") {
					base->children.insert(base->children.end(), loopStartIter, std::prev(iter, 1));
					continue; // after namespace name we found no bracket
				}

				auto namespaceItem = std::make_shared<Namespace>();
				namespaceItem->name = std::dynamic_pointer_cast<GenericCode>(*namespaceNameIter)->data;
				namespaceItem->children = bracketIter->get()->children;
				base->children.push_back(namespaceItem);
			}

			std::for_each(base->children.begin(), base->children.end(), &Namespace::transformNamespaces);
		}

		private: static bool isNonWhitespace(std::shared_ptr<AbstractContent> item) {
			if (std::dynamic_pointer_cast<GenericCode>(item) && (std::dynamic_pointer_cast<GenericCode>(item)->isComment() || std::dynamic_pointer_cast<GenericCode>(item)->isWhitespace())) {
				return false; // skip comments and whitespace
			}
			return true; // accept anything else. Validated later.
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

		public: virtual void optimize()
		{
			auto splittedChildren = this->splitChildren();
			this->children = {};
			for (auto& childList : splittedChildren) {
				this->children.push_back(this->convert(childList));
			}

			AbstractContent::optimize();
		}

		/**
		 * split by entity ends like ; and {}
		 */
		private: std::list<std::list<std::shared_ptr<AbstractContent>>> splitChildren()
		{
			std::list<std::list<std::shared_ptr<AbstractContent>>> result;

			result.push_back({});

			for (auto& child : this->children) {
				result.back().push_back(child);

				auto childAsBracket = dynamic_cast<Bracket*>(child.get());
				if (childAsBracket != nullptr && childAsBracket->openingBracket == "{") {
					result.push_back({});
				}

				auto childAsGeneric = dynamic_cast<GenericCode*>(child.get());
				if (childAsGeneric != nullptr && childAsGeneric->type == "commandSeparator") {
					result.push_back({});
				}
			}

			return result;
		}

		private: std::shared_ptr<AbstractContent> convert(std::list<std::shared_ptr<AbstractContent>> list)
		{
			bool isFunction = false;
			if (std::dynamic_pointer_cast<Bracket>(list.back()) && std::dynamic_pointer_cast<Bracket>(list.back())->openingBracket == "{") {
				for (auto& child : list) {
					auto childAsBracket = std::dynamic_pointer_cast<Bracket>(child);
					if (childAsBracket && childAsBracket->openingBracket == "(") {
						isFunction = true;
					}
				}
			}

			if (isFunction) {
				// use first round brackets as parameter list
				std::shared_ptr<Bracket> parameterList = nullptr;
				for (auto& child : list) {
					if (std::dynamic_pointer_cast<Bracket>(child) && std::dynamic_pointer_cast<Bracket>(child)->openingBracket == "(") {
						parameterList = std::dynamic_pointer_cast<Bracket>(child);
						break;
					}
				}

				auto result = std::make_shared<Function>();
				result->children = list.back()->children; // use contents of closing bracket
				list.pop_back();
				// read until parameter list
				while (list.size() > 0 && list.back() != parameterList) {
					result->intermediateStuff.push_front(list.back());
					list.pop_back();
				}
				if (list.size() == 0) {
					throw std::runtime_error("function read failed - parameter list not found");
				}
				result->parameterList = std::dynamic_pointer_cast<Bracket>(list.back());
				list.pop_back();

				// read until first word (function name)
				while (list.size() > 0 && (!std::dynamic_pointer_cast<GenericCode>(list.back()) || std::dynamic_pointer_cast<GenericCode>(list.back())->type != "word")) {
					list.pop_back();
				}
				if (list.size() == 0) {
					throw std::runtime_error("function read failed - name not found");
				}
				result->name = dynamic_cast<GenericCode*>(list.back().get())->data;
				list.pop_back();

				// read return value
				while (list.size() > 0 && (!std::dynamic_pointer_cast<GenericCode>(list.back()) || std::dynamic_pointer_cast<GenericCode>(list.back())->type != "accessControl")) {
					result->returnValue.push_front(list.back());
					list.pop_back();
				}
				if (list.size() != 0) {
					result->access = std::dynamic_pointer_cast<GenericCode>(list.back());
					list.pop_back();
				}

				while (list.size() != 0) {
					result->beforeAccess.push_front(list.back());
					list.pop_back();
				}

				return result;
			} else {
				auto result = std::make_shared<Property>();
				result->children = list;
				return result;
			}
		}

		public: static std::shared_ptr<Class> readFromString(std::string const& content, size_t& pos)
		{
			std::shared_ptr<Class> result = nullptr;

			if (getNextWord(content, pos) == "class") {
				pos += std::string("class").size();
				result = std::make_shared<Class>();
				size_t contentBracketPos = content.find_first_of('{', pos);
				std::string nameAndProperties = content.substr(pos, contentBracketPos - pos - 1);
				size_t colonPos = nameAndProperties.find_first_of(':');
				if (colonPos == -1) { // no properties set
					result->name = nameAndProperties;
				} else {
					result->name = GcBuild::trim(nameAndProperties.substr(0, colonPos));
					result->properties = GcBuild::trim(nameAndProperties.substr(colonPos));
				}
				pos = contentBracketPos + 1;
			}

			return result;
		}

		public: virtual std::string render(std::list<std::string> path) override
		{
			path.push_back(this->getPathPart());
			return "class " + this->name + this->properties + "\n{ " + AbstractContent::render(path) + "}";
		}

		public: virtual std::string getPathPart() override
		{
			return this->name;
		}

	};

	class File : public AbstractContent
	{
		public: virtual ~File(){}
		public: virtual std::string describe()
		{
			return "file";
		}

		public: std::string renderSource()
		{
			std::string result;

			for (auto& functionAndPath : this->findFunctions()) {
				result += std::dynamic_pointer_cast<Function>(functionAndPath.first)->render(RenderDest::SOURCE, functionAndPath.second);
			}

			return result;
		}
	};
	
	class Parser
	{
		private: std::list<std::function<std::shared_ptr<AbstractContent>(std::string const& content, size_t& pos)>> readers;

		public: Parser()
		{
			this->readers = {
				&GenericCode::readCommentFromString,
				&GenericCode::readStringFromString,
				&Bracket::readFromString,
				&ClosingBracket::readFromString,
				&GenericCode::readPreprocessorFromString,
				&GenericCode::readAccessControlFromString,
				&Class::readFromString,
				&GenericCode::readWordFromString,
				&GenericCode::readCommandSeparatorFromString,
				&GenericCode::readCharFromString
			};
		}

		public: std::shared_ptr<File> parse(std::string const& content)
		{
			size_t pos = 0;

			std::shared_ptr<File> result = std::make_shared<File>();

			auto readerIter = this->readers.begin();

			while (readerIter != this->readers.end()) {
				auto reader = *readerIter;
				auto subResult = reader(content, pos);
				if (subResult) {
					result->children.push_back(subResult);
					readerIter = this->readers.begin();
				} else {
					readerIter++;
				}
			}

			return result;
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

			auto file = std::make_shared<Parser>()->parse(content);
			file->groupChars();
			file->groupContainers();
			Namespace::transformNamespaces(file);
			file->dumpTree();
//			file->optimize();

//			putFileContents(this->root + "/" + headerDest, file->render({}));
//			putFileContents(this->root + "/" + implDest, file->renderSource());
		}
	};

	void prepareDirectory(std::string const& basePath, std::string const& file)
	{
		auto parts = split(file, "/");
		parts.pop_back();
		std::string path = "";
		for (auto& part : parts) {
			path += part + "/";
			mkdir((basePath + "/" + path).c_str(), 0777);
		}
	}
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "I need the root path of the project as argument #1" << std::endl;
		return 1;
	}

	auto optimizer = std::make_shared<GcBuild::Optimizer>(argv[1]);

	auto destPath = std::string(argv[1]) + "/build/fastbuild";

	for (int i = 2; i < argc; i++) {
		std::cout << "preparing " << argv[i] << std::endl;

		mkdir(destPath.c_str(), 0777);
		GcBuild::prepareDirectory(destPath, argv[i]);
		optimizer->optimize(
			argv[i],
			destPath + "/" + argv[i],
			GcBuild::substituteSuffix(destPath + "/" + argv[i], "cpp")
		);
	}
}
