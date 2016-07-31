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

class Resolver
{
	public: std::list<std::string> files;
	public: std::list<std::string> classes;
	public: std::map<std::string, std::string> classToFile;
	public: std::string sourcePath;

	public: std::list<std::string> resolvedIncludes;
	public: std::list<std::string> dispatchedIncludes;

	public: int verbosityLevel = 0;

	public: clock_t regexTime = 0;
	public: clock_t buildRegexTime = 0;
	public: clock_t readFileTime = 0;

	public: void scanFiles()
	{
		for (auto file : getFiles(this->sourcePath, {"hpp", "cpp"})) {
			std::string fileContent = readFile(file);
			std::regex regex("(?:^|\n)(?:[\\t ]*(?:class|struct|enum class|enum) )([^ <>\n]+)");

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
		if (level == 0) {
			this->reset();
		}
		int timeBeforeRead = clock();
		std::string fileContent = readFile(this->sourcePath + "/" + fileToResolve);
		this->readFileTime += clock() - timeBeforeRead;

		int timeBeforeBuild = clock();
		auto allClasses = std::accumulate(this->classes.begin(), this->classes.end(), std::string(),
		    [](const std::string& a, const std::string& b) -> std::string {
		        return a + (a.length() > 0 ? "|" : "") + b;
		    } );

		std::regex regex("(?:[^A-Za-z0-9_.]|^|\n)(" + allClasses + ")(?:[^A-Za-z0-9_.]|$|\n)");
		this->buildRegexTime += clock() - timeBeforeBuild;

		std::string::const_iterator searchStart(fileContent.cbegin());

		std::smatch matches;

		time_t timeBefore = clock();
		while (std::regex_search(searchStart, fileContent.cend(), matches, regex)) {
			this->regexTime += clock() - timeBefore;
			std::string file = this->classToFile[matches[1]];
			if (file != "" && std::count(this->dispatchedIncludes.begin(), this->dispatchedIncludes.end(), file) == 0) {
				this->printProgress(1, "  > found dependency: " + std::string(matches[1]) + " (processing)", level);
				this->dispatchedIncludes.push_back(file);
				if (file != fileToResolve) {
					this->printProgress(1, "[+] scanning children of " + file, level);
					resolve(file, level + 1);
					this->printProgress(1, "[-] finished scanning children of " + file, level);
				}

				if (std::count(this->resolvedIncludes.begin(), this->resolvedIncludes.end(), file) == 0
					&& file != fileToResolve) { // TODO: camparing global file to resolve really required?

					this->printProgress(1, "  + adding " + file, level);
					this->resolvedIncludes.push_back(file);
				}
			} else {
				std::string reason = file == "" ? "no file" : "already processing";
				this->printProgress(2, "  # found dependency: " + std::string(matches[1]) + " (skipping - " + reason + ")", level);
			}
			searchStart += matches.position() + matches.length();
			timeBefore = clock();
		}
		this->regexTime += clock() - timeBefore;
	}

	public: void addClass(std::string className, std::string fileName)
	{
		this->classes.push_back(className);
		this->files.push_back(fileName);
		this->classToFile[className] = fileName;
	}

	private: void reset()
	{
		this->resolvedIncludes.clear();
		this->dispatchedIncludes.clear();

		this->regexTime = 0;
		this->buildRegexTime = 0;
		this->readFileTime = 0;
	}

	private: void printProgress(int verbosityLevel, std::string info, int nestingLevel)
	{
		if (verbosityLevel <= this->verbosityLevel) {
			std::cerr << std::string(nestingLevel * 4, ' ') << info << std::endl;
		}
	}
};

int main(int argc, char** argv)
{
	std::string currentDir = get_directory(argv[0]);

	Resolver resolver;

	if (argc >= 2) {
		resolver.verbosityLevel = std::stoi(argv[1]);
	}

	resolver.addClass("assert", "lib/Helper.hpp");
	resolver.sourcePath = currentDir + "/../src";

	resolver.scanFiles();

	std::vector<std::string> baseFiles = {
		"Bootstrap/GtkApplication.cpp",
		"Bootstrap/FactoryImpl/GlibThread.cpp",
		"Bootstrap/FactoryImpl/GLibRegex.cpp",
		"Bootstrap/FactoryImpl/GlibThread.cpp",
		"Bootstrap/GtkView.cpp",
		"main/proxy.cpp",
		"main/client.cpp"
	};

	if (argc >= 3) {
		baseFiles = {argv[2]}; // override by argument for testing purposes
	}

	for (std::string fileToResolve : baseFiles) {
		std::cerr << std::endl;
		std::cerr << "#-------------------" << std::endl;
		std::cerr << "# processing " << fileToResolve << std::endl;
		std::cerr << "#-------------------" << std::endl << std::endl;

		int nestingLevel = std::count(fileToResolve.begin(), fileToResolve.end(), '/');
		std::string prefix;
		for (int i = 0; i < nestingLevel; i++) {
			prefix += "../";
		}
		std::string outFile = fileToResolve;
		outFile.replace(fileToResolve.find_last_of('.'), 0, ".inc"); // inject ".inc" before .cpp

		clock_t fullTimeBefore = clock();
		resolver.resolve(fileToResolve);

		std::cerr << std::endl << "Stats" << std::endl;
		std::cerr << "time used: " << clock() - fullTimeBefore << std::endl;
		std::cerr << "regex time: " << resolver.regexTime << std::endl;
		std::cerr << "build regex time: " << resolver.buildRegexTime << std::endl;
		std::cerr << "readFile time: " << resolver.readFileTime << std::endl;

		std::ofstream outFileStream(resolver.sourcePath + "/" + outFile, std::ofstream::out);

		for (auto file : resolver.resolvedIncludes) {
			outFileStream << "#include \"" << prefix << file << "\"" << std::endl;
		}
	}
}
