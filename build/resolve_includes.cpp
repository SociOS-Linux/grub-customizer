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
#include <sys/time.h>

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

clock_t regexTime = 0;
clock_t buildRegexTime = 0;
clock_t readFileTime = 0;

class Resolver
{
	public: std::list<std::string> files;
	public: std::list<std::string> classes;
	public: std::map<std::string, std::string> classToFile;
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
				searchStart += matches.position() + matches.length();
				if (matches[1] == "stat" || matches[1] == "dirent") {
					continue; // stat must be written as "struct stat" but it's not a structure definition
				}
				this->files.push_back(file.substr(this->sourcePath.size() + 1));
				this->classes.push_back(matches[1]);
				this->classToFile[this->classes.back()] = this->files.back();
			}
		}
	}

	public: void resolve(std::string fileToResolve, int level = 0)
	{
		int timeBeforeRead = clock();
		std::string fileContent = readFile(this->sourcePath + "/" + fileToResolve);
		readFileTime += clock() - timeBeforeRead;

		int timeBeforeBuild = clock();
		auto allClasses = std::accumulate(this->classes.begin(), this->classes.end(), std::string(),
		    [](const std::string& a, const std::string& b) -> std::string {
		        return a + (a.length() > 0 ? "|" : "") + b;
		    } );

		std::regex regex("(?:[^A-Za-z0-9_.]|^|\n)(" + allClasses + ")(?:[^A-Za-z0-9_.]|$|\n)");
		buildRegexTime += clock() - timeBeforeBuild;

		std::string::const_iterator searchStart(fileContent.cbegin());

		std::smatch matches;

		time_t timeBefore = clock();
		while (std::regex_search(searchStart, fileContent.cend(), matches, regex)) {
			regexTime += clock() - timeBefore;
			std::string file = this->classToFile[matches[1]];
			if (file != "" && std::count(this->dispatchedIncludes.begin(), this->dispatchedIncludes.end(), file) == 0) {
				std::cerr << std::string(level*4, ' ') << "  > found dependency: " << matches[1] << " (processing)" << std::endl;
				this->dispatchedIncludes.push_back(file);
				if (file != fileToResolve) {
					std::cerr << std::string(level*4, ' ') << "[+] scanning children of " << file << std::endl;
					resolve(file, level + 1);
					std::cerr << std::string(level*4, ' ') << "[-] finished scanning children of " << file << std::endl;
				}

				if (std::count(this->resolvedIncludes.begin(), this->resolvedIncludes.end(), file) == 0
					&& file != fileToResolve) { // TODO: camparing global file to resolve really required?

					std::cerr << std::string(level*4, ' ') << "  + adding " << file << std::endl;
					this->resolvedIncludes.push_back(file);
				}
			} else {
				std::string reason = file == "" ? "no file" : "already processing";
				std::cerr << std::string(level*4, ' ') << "  # found dependency: " << matches[1] << " (skipping - " << reason << ")" << std::endl;
			}
			searchStart += matches.position() + matches.length();
			timeBefore = clock();
		}
		regexTime += clock() - timeBefore;
	}

	public: void addClass(std::string className, std::string fileName)
	{
		this->classes.push_back(className);
		this->files.push_back(fileName);
		this->classToFile[className] = fileName;
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

	resolver.addClass("assert", "lib/Helper.hpp");
	resolver.sourcePath = currentDir + "/../src";

	resolver.scanFiles();
	clock_t fullTimeBefore = clock();
	resolver.resolve(fileToResolve);
	std::cerr << "time used: " << clock() - fullTimeBefore << std::endl;
	std::cerr << "regex time: " << regexTime << std::endl;
	std::cerr << "build regex time: " << buildRegexTime << std::endl;
	std::cerr << "readFile time: " << readFileTime << std::endl;

	for (auto file : resolver.resolvedIncludes) {
		std::cout << "#include \"" << prefix << file << "\"" << std::endl;
	}
}
