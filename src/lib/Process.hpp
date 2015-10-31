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

#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "Pipe.hpp"

class Process
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

	public: Process(
		std::string const& cmd,
		std::vector<std::string> const& args,
		std::map<unsigned int, Process::ChildAction> descriptorSpecs
	) {
		for (auto descriptorSpec : descriptorSpecs) {
			this->pipeConnections[descriptorSpec.first] = PipeConnection(
				descriptorSpec.first,
				descriptorSpec.second,
				std::make_shared<Pipe>()
			);
		}

		pid_t childProc = this->createNewProcess(std::bind(std::mem_fn(&Process::handleChildProcess), this, cmd, args));

		for (auto pipeConnection : this->pipeConnections) {
			if (pipeConnection.second.childAction == ChildAction::WRITE) {
				pipeConnection.second.pipe->closeWriteDescriptor();
			}
			if (pipeConnection.second.childAction == ChildAction::READ) {
				pipeConnection.second.pipe->closeReadDescriptor();
			}
		}
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

	private: void handleChildProcess(
		std::string const& cmd,
		std::vector<std::string> const& args
	) {
		for (auto pipeConnection : this->pipeConnections) {
			if (pipeConnection.second.childAction == ChildAction::WRITE) {
				pipeConnection.second.pipe->closeReadDescriptor();
				pipeConnection.second.pipe->mapWriteDescriptor(pipeConnection.second.channel);
			}
			if (pipeConnection.second.childAction == ChildAction::READ) {
				pipeConnection.second.pipe->closeWriteDescriptor();
				pipeConnection.second.pipe->mapReadDescriptor(pipeConnection.second.channel);
			}
		}

		auto argv = new char*[args.size() + 2];

		int i = 0;

		argv[i++] = const_cast<char*>(cmd.c_str());
		for (auto& arg : args) {
			argv[i++] = const_cast<char*>(arg.c_str());
		}
		argv[i++] = NULL;

		execvp(cmd.c_str(), argv);
	}
};

#endif /* SRC_LIB_PROCESS_HPP_ */
