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
 
#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <fstream>
#include <list>
#include <dirent.h>
#include <tuple>
#include <map>
#include <algorithm>

namespace Autoload
{

	std::string getFileContents(std::string const& file)
	{
		std::ifstream ifs(file.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

		std::ifstream::pos_type fileSize = ifs.tellg();

		if (fileSize == -1) {
			throw std::runtime_error("file not found: " + file);
		}

		ifs.seekg(0, std::ios::beg);

		std::vector<char> bytes(fileSize);
		ifs.read(&bytes[0], fileSize);

		return std::string(&bytes[0], fileSize);
	}

	std::string buildCharString(char from, char to)
	{
		std::string result;

		for (char c = from; c <= to; c++) {
			result += c;
		}

		return result;
	}

	bool isDir(std::string const& path)
	{
		DIR* dir = opendir(path.c_str());
		if (!dir) {
			return false;
		}
		closedir(dir);
		return true;
	}

	bool isFile(std::string const& path)
	{
		FILE* file = fopen(path.c_str(), "r");
		if (!file) {
			return false;
		}
		fclose(file);
		return true;
	}

	std::string stringRepeat(std::string const& str, unsigned int times)
	{
		std::string result;

		for (unsigned int i = 0; i < times; i++) {
			result += str;
		}

		return result;
	}

	class ClassPathParser
	{
		public: std::string alpha;

		public: ClassPathParser()
		{
			this->alpha = buildCharString('a', 'z') + buildCharString('A', 'Z') + buildCharString('0', '9');
		}

		public: std::tuple<std::list<std::string>, size_t> parse(std::string& src, size_t pos)
		{
			std::list<std::string> result;

			while (src.substr(pos, 2) == "::") {
				pos += 2;

				auto end = src.find_first_not_of(this->alpha, pos) - 1;

				auto part = src.substr(pos, end - pos + 1);

				result.push_back(part);

				pos += part.size();
			}

			return std::tuple<std::list<std::string>, size_t>(result, pos);
		}
	};

	class ClassPathResolver
	{
		private: std::string const& root;

		public: ClassPathResolver(std::string const& root)
			: root(root)
		{}

		public: std::string getFileName(std::list<std::string> const& path)
		{
			std::string result = "src";

			for (auto const& fileName : path) {
				std::string newPath = result + "/" + fileName;
				if (isDir(this->root + "/" + newPath)) {
					result = newPath;
				} else if (isFile(this->root + "/" + newPath + ".hpp")) {
					return newPath + ".hpp";
				} else {
					throw std::runtime_error("cannot find file/directory: " + newPath);
				}
			}
			throw std::runtime_error("path resolution failed. Current part: " + result);
		}
	};

	class AutoloadBuilder
	{
		private: std::string root;

		private: std::map<std::string, std::list<std::string>> dependencyMap;

		public: AutoloadBuilder(std::string const& root)
			: root(root)
		{}

		public: void build(std::string const& fileName)
		{
			this->resolveDependenciesRecursive(fileName);
			//this->dumpDependencyMap();

			auto aggregated = this->aggregateDependencies(fileName);

			this->writeToFile(aggregated, fileName + ".inc.hpp");
		}

		private: std::list<std::string> getDependencies(std::string const& fileName)
		{
			std::list<std::string> dependencies;

			std::string content = getFileContents(this->root + "/" + fileName);

			size_t pos = 0;
			while ((pos = content.find("Gc::", pos)) != -1) {
				auto pathParser = std::make_shared<ClassPathParser>();
				std::list<std::string> path;
				std::tie(path, pos) = pathParser->parse(content, pos + 2);

				auto pathResolver = std::make_shared<ClassPathResolver>(root);
				auto file = pathResolver->getFileName(path);

				if (file == fileName) { // cannot have itself as dependency
					continue;
				}
				if (std::find(dependencies.begin(), dependencies.end(), file) == dependencies.end()) {
					dependencies.push_back(file);
				}
			}

			return dependencies;
		}

		private: void resolveDependenciesRecursive(std::string const& fileName)
		{
			auto dependencies = this->getDependencies(fileName);
			this->dependencyMap[fileName] = dependencies;

			for (auto const& dependency : dependencies) {
				if (this->dependencyMap.find(dependency) == this->dependencyMap.end()) {
					this->resolveDependenciesRecursive(dependency);
				}
			}
		}

		private: std::list<std::string> aggregateDependencies(
			std::string const& fileName,
			std::shared_ptr<std::list<std::string>> track = nullptr
		) {
			if (track == nullptr) {
				track = std::make_shared<std::list<std::string>>();
			}
			std::list<std::string> result;
			for (auto dependency : this->dependencyMap[fileName]) {
				if (std::find(track->begin(), track->end(), dependency) == track->end()) {
					track->push_back(dependency);

					result.splice(result.end(), this->aggregateDependencies(dependency, track));
					result.push_back(dependency);
				}
			}
			return result;
		}

		private: void dumpDependencyMap()
		{
			for (auto& mapItem : this->dependencyMap) {
				std::cout << mapItem.first << ":" << std::endl;
				for (auto& dependencyFile : mapItem.second) {
					std::cout << "\t" << dependencyFile << std::endl;
				}
			}
		}

		private: void writeToFile(std::list<std::string> const& fileList, std::string const& outFile)
		{
			auto depth = std::count(outFile.begin(), outFile.end(), '/');

			std::ofstream os;
			os.open((this->root + "/" + outFile).c_str(), std::ios::out);
			for (auto const& file : fileList) {
				os << "#include \"" << stringRepeat("../", depth) + file << "\"\n";
			}
			os.close();
		}

	};
}
 
int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "I need the root path of the project as argument #1" << std::endl;
		return 1;
	}

	auto autoloadBuilder = std::make_shared<Autoload::AutoloadBuilder>(argv[1]);

	autoloadBuilder->build("src/main/client.cpp");
	autoloadBuilder->build("src/Bootstrap/GtkView.cpp");
	autoloadBuilder->build("src/Bootstrap/GtkApplication.cpp");
	autoloadBuilder->build("src/Bootstrap/FactoryImpl/GlibThread.cpp");
	autoloadBuilder->build("src/Bootstrap/FactoryImpl/GLibRegex.cpp");
	autoloadBuilder->build("src/main/proxy.cpp");
}
