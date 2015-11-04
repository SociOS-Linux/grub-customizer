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

#ifndef SRC_LIB_PROCESS_HPP_
#define SRC_LIB_PROCESS_HPP_

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <set>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include <iostream>

#include "Pipe.hpp"

class Process : public std::enable_shared_from_this<Process>
{
	public: enum StdChannels {
		STDIN  = 0,
		STDOUT = 1,
		STDERR = 2
	};

	public: enum ForkPids {
		CHILD = 0,
		ERROR = -1
	};

	public: enum ChildAction {
		READ = 'r',
		WRITE = 'w'
	};

	public: enum FileWriteMode {
		REPLACE,
		APPEND
	};

	public: class PipeConnection
	{
		public: unsigned int channel;
		public: std::shared_ptr<Pipe> pipe;
		public: Process::ChildAction childAction;

		public: PipeConnection() : childAction(ChildAction::READ), channel(0) {}

		public: PipeConnection(unsigned int fileDescriptorToMap, Process::ChildAction childAction, std::shared_ptr<Pipe> pipe) :
			channel(fileDescriptorToMap),
			childAction(childAction),
			pipe(pipe)
		{}
	};

	private: std::map<unsigned int, PipeConnection> pipeConnections;
	private: std::string cmd;
	private: std::vector<std::string> args;
	private: std::map<unsigned int, std::shared_ptr<Process>> pipeDest;
	private: std::map<unsigned int, std::string> inputFiles;
	private: std::map<unsigned int, std::string> outputFiles;
	private: std::map<unsigned int, FileWriteMode> outputFilesAppendFlags;
	private: std::map<unsigned int, unsigned int> channelMappings; // key: from, value: to
	private: std::set<unsigned int> passThruChannels;

	private: pid_t processId;

	protected: Process() : processId(-1) {}

	public: static std::shared_ptr<Process> create(std::string const& cmd)
	{
		std::shared_ptr<Process> proc = nullptr;

		auto procPtr = new Process();
		try {
			proc = std::shared_ptr<Process>(procPtr);
		} catch (std::bad_alloc const& e) {
			delete procPtr;
			throw e;
		}

		proc->cmd = cmd;
		return proc;
	}

	public: std::shared_ptr<Process> setCmd(std::string const& cmd)
	{
		this->cmd = cmd;
		return shared_from_this();
	}

	public: std::shared_ptr<Process> setArguments(std::vector<std::string> const& args)
	{
		this->args = args;
		return shared_from_this();
	}

	public: std::shared_ptr<Process> addArgument(std::string const& arg)
	{
		this->args.push_back(arg);
		return shared_from_this();
	}

	public: std::shared_ptr<Process> addPipe(
		unsigned int fileDescriptorToMap,
		Process::ChildAction childAction,
		std::shared_ptr<Pipe> pipe
	) {
		auto existingPipeConnectionWithSamePipeIter = std::find_if(
			this->pipeConnections.begin(),
			this->pipeConnections.end(),
			[pipe](std::pair<const unsigned int,PipeConnection> pipeCon){return pipeCon.second.pipe == pipe;}
		);

		if (existingPipeConnectionWithSamePipeIter != this->pipeConnections.end()) {
			// allow combining multiple output channels (example: read stdout and stderr from the same pipe)
			if (existingPipeConnectionWithSamePipeIter->second.childAction != childAction) {
				throw std::logic_error("pipe already used but with other childAction");
			}

			this->channelMappings[fileDescriptorToMap] = existingPipeConnectionWithSamePipeIter->second.channel;
		} else {
			this->pipeConnections[fileDescriptorToMap] = PipeConnection(
				fileDescriptorToMap,
				childAction,
				pipe
			);
		}
		return shared_from_this();
	}

	/**
	 * comfort function to easily assign stdin
	 */
	public: std::shared_ptr<Process> setStdIn(std::shared_ptr<Pipe> pipe)
	{
		return this->addPipe(Process::STDIN, Process::ChildAction::READ, pipe);
	}

	/**
	 * comfort function to easily assign stdout
	 */
	public: std::shared_ptr<Process> setStdOut(std::shared_ptr<Pipe> pipe)
	{
		return this->addPipe(Process::STDOUT, Process::ChildAction::WRITE, pipe);
	}

	/**
	 * comfort function to easily assign stderr
	 */
	public: std::shared_ptr<Process> setStdErr(std::shared_ptr<Pipe> pipe)
	{
		return this->addPipe(Process::STDERR, Process::ChildAction::WRITE, pipe);
	}

	public: std::shared_ptr<Process> addFile(
		std::string const& filePath,
		unsigned int fileDescriptor,
		Process::ChildAction childAction,
		FileWriteMode writeMode = FileWriteMode::REPLACE
	) {
		if (childAction == Process::ChildAction::READ) {
			this->inputFiles[fileDescriptor] = filePath;
		} else {
			this->outputFiles[fileDescriptor] = filePath;
			this->outputFilesAppendFlags[fileDescriptor] = writeMode;
		}

		return shared_from_this();
	}

	/**
	 * comfort function to easily assign stdin
	 */
	public: std::shared_ptr<Process> setStdIn(std::string const& filePath)
	{
		return this->addFile(filePath, Process::STDIN, Process::ChildAction::READ);
	}

	/**
	 * comfort function to easily assign stdout
	 */
	public: std::shared_ptr<Process> setStdOut(std::string const& filePath, FileWriteMode writeMode = FileWriteMode::REPLACE)
	{
		return this->addFile(filePath, Process::STDOUT, Process::ChildAction::WRITE, writeMode);
	}

	/**
	 * comfort function to easily assign stderr
	 */
	public: std::shared_ptr<Process> setStdErr(std::string const& filePath, FileWriteMode writeMode = FileWriteMode::REPLACE)
	{
		return this->addFile(filePath, Process::STDERR, Process::ChildAction::WRITE, writeMode);
	}

	public: std::shared_ptr<Process> setPassThru(std::set<unsigned int> channels = {STDIN, STDOUT, STDERR})
	{
		this->passThruChannels = channels;
		return shared_from_this();
	}

	/**
	 * comfort function to attach another process as pipe target
	 */
	public: std::shared_ptr<Process> pipeInto(
		std::shared_ptr<Process> otherProcess,
		unsigned int channelSrc = Process::STDOUT,
		unsigned int channelDest = Process::STDIN
	) {
		auto procToProcPipe = std::make_shared<Pipe>();
		this->addPipe(channelSrc, ChildAction::WRITE, procToProcPipe);
		otherProcess->addPipe(channelDest, ChildAction::READ, procToProcPipe);
		this->pipeDest[channelSrc] = otherProcess;
		return shared_from_this();
	}

	public: std::shared_ptr<Process> run()
	{
		this->processId = this->createNewProcess(std::bind(std::mem_fn(&Process::handleChildProcess), this));

		for (auto pipeConnection : this->pipeConnections) {
			if (pipeConnection.second.childAction == ChildAction::WRITE) {
				pipeConnection.second.pipe->closeWriteDescriptor();
			}
			if (pipeConnection.second.childAction == ChildAction::READ) {
				pipeConnection.second.pipe->closeReadDescriptor();
			}
		}

		for (auto pipeDest : this->pipeDest) {
			pipeDest.second->run();
		}
		return shared_from_this();
	}

	public: int finish()
	{
		int res = 0;
		::waitpid(this->processId, &res, 0);
		return res;
	}

	public: std::shared_ptr<Pipe> getPipe(unsigned int channel)
	{
		return this->pipeConnections[channel].pipe;
	}

	private: pid_t createNewProcess(std::function<void()> childProcess)
	{
		pid_t pid = ::fork();

		if (pid == ForkPids::CHILD) {
			childProcess();
		}

		if (pid == ForkPids::ERROR) {
			throw std::runtime_error("fork failed");
		}

		return pid;
	}

	private: void handleChildProcess()
	{
		for (auto pipeConnection : this->pipeConnections) {
			if (pipeConnection.second.childAction == ChildAction::WRITE) {
				pipeConnection.second.pipe->mapWriteDescriptor(pipeConnection.second.channel);
			}
			if (pipeConnection.second.childAction == ChildAction::READ) {
				pipeConnection.second.pipe->mapReadDescriptor(pipeConnection.second.channel);
			}
		}

		for (auto inputFile : this->inputFiles) {
			int file = ::open(inputFile.second.c_str(), 0);
			if (file == -1) {std::cerr << "cannot open input file!" << std::endl; _exit(1);}
			::dup2(file, inputFile.first);
			::close(file);
		}

		for (auto outputFile : this->outputFiles) {
			bool replace = this->outputFilesAppendFlags[outputFile.first] == FileWriteMode::REPLACE;
			int file = ::open(
				outputFile.second.c_str(),
				O_WRONLY | O_CREAT | (replace ? O_TRUNC : O_APPEND)
			);
			if (file == -1) {std::cerr << "cannot open output file!" << std::endl; _exit(1);}
			::dup2(file, outputFile.first);
			::close(file);
		}

		for (auto mapping : this->channelMappings) {
			::dup2(mapping.second, mapping.first);
		}

		// close all unnecessary file descriptors
		auto usedFileDescriptors = this->getAllChildFileDescriptors();
		rlimit rlim;
		getrlimit(RLIMIT_NOFILE, &rlim);
		for (rlim_t i = 0; i < rlim.rlim_cur; i++) {
			if (usedFileDescriptors.count(i) == 0) {
				// ignoring file descriptors that should be used by child process
				::close(i);
			}
		}

		auto argv = new char*[this->args.size() + 2];

		int i = 0;

		argv[i++] = const_cast<char*>(this->cmd.c_str());
		for (auto& arg : this->args) {
			argv[i++] = const_cast<char*>(arg.c_str());
		}
		argv[i++] = NULL;

		::execvp(cmd.c_str(), argv);
	}

	private: std::set<unsigned int> getAllChildFileDescriptors()
	{
		std::set<unsigned int> result;
		for (auto pipeCnn : this->pipeConnections) {
			result.insert(pipeCnn.first);
		}
		for (auto inputFile : this->inputFiles) {
			result.insert(inputFile.first);
		}
		for (auto outputFile : this->outputFiles) {
			result.insert(outputFile.first);
		}
		for (auto channelMapping : this->channelMappings) {
			result.insert(channelMapping.first);
		}
		for (auto passThruChannel : this->passThruChannels) {
			result.insert(passThruChannel);
		}
		return result;
	}
};

#endif /* SRC_LIB_PROCESS_HPP_ */
