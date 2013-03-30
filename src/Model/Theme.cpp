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

#include "Theme.h"

Model_Theme::Model_Theme(std::string const& directory, std::string const& zipFile, std::string const& name)
	: directory(directory), name(name), zipFile(zipFile)
{
	if (directory != "") {
		this->load(directory);
	}

	if (zipFile != "") {
		this->loadZipFile(zipFile);
	}
}

void Model_Theme::load(std::string const& directory) {
	DIR* dir = opendir(directory.c_str());
	if (dir) {
		struct dirent *entry;
		struct stat fileProperties;
		while ((entry = readdir(dir))) {
			if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
				continue;
			}
			std::string currentFileName = directory + "/" + entry->d_name;
			stat(currentFileName.c_str(), &fileProperties);
			if (S_ISDIR(fileProperties.st_mode)) {
				this->load(currentFileName);
			} else {
				this->files.push_back(Model_ThemeFile(this->extractLocalPath(currentFileName)));
			}
		}
		closedir(dir);
		this->files.sort(&Model_ThemeFile::compareLocalPath);
	} else {
		throw FileReadException("cannot read the theme directory: " + this->directory);
	}
}

void Model_Theme::loadZipFile(std::string const& zipFile) {
	struct archive *a;
	struct archive_entry *entry;
	int r;

	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);
	r = archive_read_open_filename(a, zipFile.c_str(), 10240);
	if (r != ARCHIVE_OK) {
		throw InvalidFileTypeException("archive not readable", __FILE__, __LINE__);
	}
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		this->files.push_back(std::string(archive_entry_pathname(entry)));
		archive_read_data_skip(a);
	}
	r = archive_read_free(a);
	if (r != ARCHIVE_OK) {
		throw InvalidFileTypeException("archive not readable", __FILE__, __LINE__);
	}
}

std::string Model_Theme::loadFileContent(std::string localFileName) {
	if (this->getFile(localFileName).content != "") {
		return this->getFile(localFileName).content;
	}

	if (this->directory != "") {
		return this->loadFileContentFromDirectory(localFileName);
	} else if (this->zipFile != "") {
		return this->loadFileContentFromZip(localFileName);
	} else {
		throw LogicException("neither directory nor zip file set", __FILE__, __LINE__);
	}
}

std::string Model_Theme::loadFileContentFromDirectory(std::string localFileName) {
	std::string data;
	FILE* file = fopen((this->directory + "/" + localFileName).c_str(), "r");
	if (file) {
		data.reserve(10240);
		int c;
		while ((c = fgetc(file)) != EOF) {
			data += char(c);
		}
		fclose(file);
	} else {
		throw FileReadException("cannot read file: " + localFileName, __FILE__, __LINE__);
	}
	return data;
}

std::string Model_Theme::loadFileContentFromZip(std::string localFileName) {
	struct archive *a;
	struct archive_entry *entry;
	int r;
	std::string result;

	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);
	r = archive_read_open_filename(a, zipFile.c_str(), 10240);
	if (r != ARCHIVE_OK) {
		throw InvalidFileTypeException("archive not readable", __FILE__, __LINE__);
	}
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		if (std::string(archive_entry_pathname(entry)) == localFileName) {
			ssize_t size = 0;
			do {
				char data[1024];
				size = archive_read_data(a, data, 1024);
				result += std::string(data, size);
			} while (size > 0);
		}
		archive_read_data_skip(a);
	}
	r = archive_read_free(a);
	if (r != ARCHIVE_OK) {
		throw InvalidFileTypeException("archive not readable", __FILE__, __LINE__);
	}
	return result;
}

std::string Model_Theme::getFullFileName(std::string localFileName) {
	if (this->directory != "") {
		return this->directory + "/" + localFileName;
	} else {
		std::string fileContent = this->loadFileContentFromZip(localFileName);
		FILE* file = fopen("/tmp/grub-customizer_theme_preview", "w");
		if (!file) {
			throw FileSaveException("cannot write preview file to " + localFileName, __FILE__, __LINE__);
		}
		fwrite(fileContent.c_str(), fileContent.size(), fileContent.size(), file);
		fclose(file);
		return "/tmp/grub-customizer_theme_preview";
	}
}

Model_ThemeFile& Model_Theme::getFile(std::string localFileName) {
	for (std::list<Model_ThemeFile>::iterator fileIter = this->files.begin(); fileIter != this->files.end(); fileIter++) {
		if (fileIter->localFileName == localFileName) {
			return *fileIter;
		}
	}
	throw ItemNotFoundException("themefile " + localFileName + " not found!", __FILE__, __LINE__);
}

Model_ThemeFile& Model_Theme::getFileByNewName(std::string localFileName) {
	for (std::list<Model_ThemeFile>::iterator fileIter = this->files.begin(); fileIter != this->files.end(); fileIter++) {
		if (fileIter->newLocalFileName == localFileName) {
			return *fileIter;
		}
	}
	throw ItemNotFoundException("themefile " + localFileName + " not found!", __FILE__, __LINE__);
}

void Model_Theme::removeFile(Model_ThemeFile const& file) {
	for (std::list<Model_ThemeFile>::iterator fileIter = this->files.begin(); fileIter != this->files.end(); fileIter++) {
		if (&*fileIter == &file) {
			this->files.erase(fileIter);
			return;
		}
	}
	throw ItemNotFoundException("themefile " + file.localFileName + " not found!", __FILE__, __LINE__);
}


std::string Model_Theme::extractLocalPath(std::string fullPath) {
	return fullPath.substr(this->directory.size() + 1);
}
