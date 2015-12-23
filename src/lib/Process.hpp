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
#include "File.hpp"

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

	public: class PipeEndConnection
	{
		public: unsigned int channel;
		public: std::shared_ptr<Stream> pipeEnd;

		public: PipeEndConnection() : channel(0) {}

		public: PipeEndConnection(unsigned int fileDescriptorToMap, std::shared_ptr<Stream> pipeEnd) :
			channel(fileDescriptorToMap),
			pipeEnd(pipeEnd)
		{}
	};

	private: std::map<int, PipeEndConnection> pipeConnections;
	private: std::string cmd;
	private: std::vector<std::string> args;
	private: std::map<std::string, std::string> env;
	private: std::string workingDirectory;

	private: std::map<int, std::shared_ptr<Process>> pipeDest;
	private: std::set<int> passThruChannels;

	private: pid_t processId;

	private: std::shared_ptr<Pipe> errorDetector;

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

	public: std::shared_ptr<Process> addPipeEnd(
		unsigned int fileDescriptorToMap,
		std::shared_ptr<Stream> pipeEnd
	) {
		std::shared_ptr<Stream> oldPipeEnd = nullptr;
		if (this->pipeConnections.find(fileDescriptorToMap) != this->pipeConnections.end()) {
			oldPipeEnd = this->pipeConnections[fileDescriptorToMap].pipeEnd;
		}

		this->pipeConnections[fileDescriptorToMap] = PipeEndConnection(
			fileDescriptorToMap,
			pipeEnd
		);
		pipeEnd->registerUsage();

		if (oldPipeEnd != nullptr) {
			oldPipeEnd->close(); // decrease usage count or close if no more used
		}

		return shared_from_this();
	}

	/**
	 * comfort function to easily assign stdin
	 */
	public: std::shared_ptr<Process> setStdIn(std::shared_ptr<InputStream> pipeEnd)
	{
		return this->addPipeEnd(Process::STDIN, pipeEnd);
	}

	/**
	 * comfort function to easily assign stdout
	 */
	public: std::shared_ptr<Process> setStdOut(std::shared_ptr<OutputStream> pipeEnd)
	{
		return this->addPipeEnd(Process::STDOUT, pipeEnd);
	}

	/**
	 * comfort function to easily assign stderr
	 */
	public: std::shared_ptr<Process> setStdErr(std::shared_ptr<OutputStream> pipeEnd)
	{
		return this->addPipeEnd(Process::STDERR, pipeEnd);
	}

	public: std::shared_ptr<Process> addInputFile(
		std::string const& filePath,
		unsigned int fileDescriptor
	) {
		this->addPipeEnd(fileDescriptor, File::openInputFile(filePath));
		return shared_from_this();
	}

	public: std::shared_ptr<Process> addOutputFile(
		std::string const& filePath,
		unsigned int fileDescriptor,
		File::WriteMode writeMode = File::WriteMode::REPLACE
	) {
		this->addPipeEnd(fileDescriptor, File::openOutputFile(filePath, writeMode));
		return shared_from_this();
	}

	/**
	 * comfort function to easily assign stdin
	 */
	public: std::shared_ptr<Process> setStdIn(std::string const& filePath)
	{
		return this->addInputFile(filePath, Process::STDIN);
	}

	/**
	 * comfort function to easily assign stdout
	 */
	public: std::shared_ptr<Process> setStdOut(std::string const& filePath, File::WriteMode writeMode = File::WriteMode::REPLACE)
	{
		return this->addOutputFile(filePath, Process::STDOUT, writeMode);
	}

	/**
	 * comfort function to easily assign stderr
	 */
	public: std::shared_ptr<Process> setStdErr(std::string const& filePath, File::WriteMode writeMode = File::WriteMode::REPLACE)
	{
		return this->addOutputFile(filePath, Process::STDERR, writeMode);
	}

	public: std::shared_ptr<Process> setPassThru(std::set<int> channels = {STDIN, STDOUT, STDERR})
	{
		this->passThruChannels = channels;
		return shared_from_this();
	}

	public: std::shared_ptr<Process> setEnv(std::map<std::string, std::string> const& env)
	{
		this->env = env;
		return shared_from_this();
	}

	public: std::shared_ptr<Process> addEnv(std::string const& key, std::string const& value)
	{
		this->env[key] = value;
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
		this->addPipeEnd(channelSrc, procToProcPipe->getWriter());
		otherProcess->addPipeEnd(channelDest, procToProcPipe->getReader());
		this->pipeDest[channelSrc] = otherProcess;
		return shared_from_this();
	}

	/**
	 * comfort function to easily assign stdout
	 */
	public: std::shared_ptr<Process> setStdOut(std::shared_ptr<Process> otherProcess)
	{
		return this->pipeInto(otherProcess, Process::STDOUT);
	}

	/**
	 * comfort function to easily assign stderr
	 */
	public: std::shared_ptr<Process> setStdErr(std::shared_ptr<Process> otherProcess)
	{
		return this->pipeInto(otherProcess, Process::STDERR);
	}

	public: std::shared_ptr<Process> setWorkingDirectory(std::string const& workingDirectory)
	{
		this->workingDirectory = workingDirectory;
		return shared_from_this();
	}

	public: std::shared_ptr<Process> run()
	{
		this->errorDetector = std::make_shared<Pipe>();

		this->processId = this->createNewProcess(std::bind(std::mem_fn(&Process::handleChildProcess), this));

		for (auto pipeConnection : this->pipeConnections) {
			pipeConnection.second.pipeEnd->close();
		}

		this->errorDetector->getWriter()->close();

		for (auto pipeDest : this->pipeDest) {
			pipeDest.second->run();
		}

		// this pipe gets data only when the child process didn't start correctly
		std::string errorMessage = this->errorDetector->getReader()->read(50);
		if (errorMessage.size()) {
			throw std::runtime_error(errorMessage);
		}

		return shared_from_this();
	}

	public: void kill(int signal = SIGTERM)
	{
		int suc = ::kill(this->processId, signal);
		if (suc == -1) {
			throw std::runtime_error("failed killing process");
		}
	}

	public: int finish()
	{
		int res = 0;
		::waitpid(this->processId, &res, 0);
		return res;
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
		if (fcntl(this->errorDetector->getWriter()->getDescriptor(), F_SETFD, FD_CLOEXEC) == -1) {
			throw std::runtime_error("pipe setup failed");
		}

		this->pipeConnections[-1] = PipeEndConnection(
			-1,
			this->errorDetector->getWriter()
		);

		try {
			this->forwardIntoProcess();
		} catch (std::exception const& e) {
			this->errorDetector->getWriter()->write(e.what());
			this->errorDetector->getWriter()->close();
			::_exit(1);
		}
	}

	public: void forwardIntoProcess()
	{
		// copy channels to a new number if equaling with desired number
		for (auto pipeConnection : this->pipeConnections) {
			while (this->pipeConnections.find(pipeConnection.second.pipeEnd->getDescriptor()) != this->pipeConnections.end()) {
				// if current number is a desired number: find a new channel (repeat until it's really unused)
				pipeConnection.second.pipeEnd->setDescriptor(::dup(pipeConnection.second.pipeEnd->getDescriptor()));
				if (pipeConnection.second.pipeEnd->getDescriptor() == -1) {
					throw std::runtime_error("pipe setup failed");
				}
				// not closed to prevent allocation by next dup() call
			}
		}

		std::set<int> usedChannels = this->passThruChannels;

		for (auto pipeConnection : this->pipeConnections) {
			if (pipeConnection.first < 0) { // channels < 0 (means: no desired number) wont be mapped
				usedChannels.insert(pipeConnection.second.pipeEnd->getDescriptor());
				continue;
			}
			::close(pipeConnection.first);
			pipeConnection.second.pipeEnd->map(pipeConnection.first);
			usedChannels.insert(pipeConnection.first);
		}

		// close all unnecessary file descriptors
		rlimit rlim;
		getrlimit(RLIMIT_NOFILE, &rlim);
		for (rlim_t i = 0; i < rlim.rlim_cur; i++) {
			if (usedChannels.count(i) == 0) {
				// ignoring file descriptors that should be used by child process
				::close(i);
			}
		}

		if (this->workingDirectory.size()) {
			chdir(this->workingDirectory.c_str());
		}

		auto argv = new char*[this->args.size() + 2];

		int i = 0;

		argv[i++] = const_cast<char*>(this->cmd.c_str());
		for (auto& arg : this->args) {
			argv[i++] = const_cast<char*>(arg.c_str());
		}
		argv[i++] = NULL;

		if (this->env.size()) {
			int i = 0;
			auto env = new char*[this->env.size() + 1];
			for (auto& envItem : this->env) {
				env[i++] = const_cast<char*>((envItem.first + "=" + envItem.second).c_str());
			}
			env[i++] = NULL;
			::execvpe(this->cmd.c_str(), argv, env);
		} else {
			::execvp(this->cmd.c_str(), argv);
		}

		// error handling
		throw std::runtime_error("cannot execute command \"" + this->cmd + "\"!");
	}
};

#endif /* SRC_LIB_PROCESS_HPP_ */
