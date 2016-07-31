#include <fstream>
#include <list>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <glob.h>
#include <sys/types.h>
#include <dirent.h>
#include <regex>
#include <algorithm>

std::string get_directory(std::string argv0)
{
	auto readlink = popen(("dirname \"`readlink -f \"" + std::string(argv0) + "\"`\"").c_str(), "r");

	std::string path;
	int c = 0;
	while ((c = fgetc(readlink)) != EOF) {
		if (c == '\n') {
			continue;
		}
		path += char(c);
	}
	pclose(readlink);

	return path;
}

std::list<std::string> getFiles(std::string directory, std::list<std::string> fileExtensions) {
	std::list<std::string> result;

	DIR* dir = opendir(directory.c_str());

	dirent* file = nullptr;
	while ((file = readdir(dir)) != nullptr) {
		std::string fileName = file->d_name;

		if (fileName == "." || fileName == "..") {
			continue;
		}
		if (file->d_type == DT_DIR) {
			result.splice(result.end(), getFiles(directory + "/" + fileName, fileExtensions));
		} else {
			for (auto extension : fileExtensions) {
				if (fileName.substr(fileName.size() - extension.size()) == extension) {
					result.push_back(directory + "/" + fileName);
					break;
				}
			}
		}
	}

	return result;
}

std::string readFile(std::string file)
{
	std::ifstream ifs(file.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	std::ifstream::pos_type fileSize = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	std::vector<char> bytes(fileSize);
	ifs.read(&bytes[0], fileSize);

	return std::string(&bytes[0], fileSize);
}

int regexTime = 0;
int buildRegexTime = 0;
int readFileTime = 0;

class Resolver
{
	public: std::list<std::string> files;
	public: std::list<std::string> classes;
	public: std::string sourcePath;

	public: std::list<std::string> resolvedIncludes;
	public: std::list<std::string> dispatchedIncludes;

	public: void scanFiles()
	{
		for (auto file : getFiles(this->sourcePath, {"hpp", "cpp"})) {
			std::string fileContent = readFile(file);
			std::regex regex("(?:^|\n)(?:[\\t ]*(?:class|struct) )([^ <>\n]+)");

			std::smatch matches;
			std::string::const_iterator searchStart(fileContent.cbegin());
			while (std::regex_search(searchStart, fileContent.cend(), matches, regex)) {
				this->files.push_back(file.substr(this->sourcePath.size() + 1));
				this->classes.push_back(matches[1]);
				searchStart += matches.position() + matches.length();
			}
		}
	}

	public: void resolve(std::string fileToResolve)
	{
		int timeBeforeRead = time(NULL);
		std::string fileContent = readFile(this->sourcePath + "/" + fileToResolve);
		readFileTime += time(NULL) - timeBeforeRead;

		auto fileIter = this->files.begin();
		auto classIter = this->classes.begin();
		for (int i = 0; i < this->files.size(); i++) {
			int timeBeforeBuild = time(NULL);
			std::regex regex("(?:[^A-Za-z0-9_.]|^|\n)" + *classIter + "(?:[^A-Za-z0-9_.]|$|\n)");
			buildRegexTime += time(NULL) - timeBeforeBuild;

			int timeBefore = time(NULL);
			if (std::regex_search(fileContent, regex)) {
				regexTime += time(NULL) - timeBefore;
				if (std::count(this->dispatchedIncludes.begin(), this->dispatchedIncludes.end(), *fileIter) == 0) {
					this->dispatchedIncludes.push_back(*fileIter);
					if (*fileIter != fileToResolve) {
						std::cerr << "[+] scanning children of " << *fileIter << " (POS: " << i << ")" << std::endl;
						resolve(*fileIter);
						std::cerr << "[-] finished scanning children of " << *fileIter << " (POS: " << i << ")" << std::endl;
					}

					if (std::count(this->resolvedIncludes.begin(), this->resolvedIncludes.end(), *fileIter) == 0
						&& *fileIter != fileToResolve) { // TODO: camparing global file to resolve really required?

						std::cerr << "adding " << *fileIter << std::endl;
						this->resolvedIncludes.push_back(*fileIter);
					}
				}
			} else {
				regexTime += time(NULL) - timeBefore;
			}
			fileIter++;
			classIter++;
		}
	}
};

int main(int argc, char** argv)
{
	std::string currentDir = get_directory(argv[0]);


	if (argc != 3) {
		std::cerr << "Error: wrong argument count" << std::endl;
		return 1;
	}

	Resolver resolver;

	std::string fileToResolve = argv[1];
	std::string prefix = argv[2];

	resolver.files = {"lib/Helper.hpp"};
	resolver.classes = {"assert"};
	resolver.sourcePath = currentDir + "/../src";

	resolver.scanFiles();
	int fullTimeBefore = time(NULL);
	resolver.resolve(fileToResolve);
	std::cerr << "time used: " << time(NULL) - fullTimeBefore << std::endl;
	std::cerr << "regex time: " << regexTime << std::endl;
	std::cerr << "build regex time: " << buildRegexTime << std::endl;
	std::cerr << "readFile time: " << readFileTime << std::endl;

	for (auto file : resolver.resolvedIncludes) {
		std::cout << "#include \"" << prefix << file << "\"" << std::endl;
	}
}
