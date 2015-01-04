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

#include "../Model/Mapper/Abstract/ProcessReader.hpp"
#include <memory>
#include <glibmm.h>
#include <gtkmm.h>
#include "../Controller/Helper/GLibThread.hpp"
#include "../lib/Mutex/GLib.hpp"

void execCommand(Gtk::TextView& dest)
{
	dest.get_buffer()->set_text("");

	Model_Mapper_Abstract_ProcessReader pr;
	pr.setThreadHelper(std::make_shared<Controller_Helper_GLibThread>());
	pr.setMutex(std::make_shared<Mutex_GLib>());

	auto buf = std::make_shared<std::string>();

	pr.runASync(
		"grub-mkconfig",
		[&dest, buf] (char receivedChar) {
			*buf += receivedChar;

			if (receivedChar == '\n') {
				dest.get_buffer()->insert(dest.get_buffer()->begin(), *buf);
				*buf = "";
			}
		},
		[&dest, buf] (int status) {
			dest.get_buffer()->insert(dest.get_buffer()->begin(), *buf);
			dest.get_buffer()->insert(dest.get_buffer()->begin(), "terminated with exit status " + std::to_string(status));
		}
	);
}

class Test : public std::enable_shared_from_this<Test>
{
	public: void checkIsShared()
	{
		try {
			auto ptr = this->shared_from_this();
			std::cout << "object is a shared_ptr" << std::endl;
		} catch (std::bad_weak_ptr const& e) {
			std::cout << "object is NOT a shared_ptr" << std::endl;
		}
	}
};

#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <array>
#include <cassert>

class Process
{
	public: enum class Channel {
		STDIN,
		STDOUT,
		STDERR
	};

	private: std::initializer_list<Channel> allChannels = {Channel::STDOUT, Channel::STDERR, Channel::STDIN};

	private: const int PIPE_INDEX_READ = 0;
	private: const int PIPE_INDEX_WRITE = 1;

	private: pid_t pid = 0;
	private: std::map<Channel, std::array<int, 2>> pipes;
	private: std::map<Channel, bool> pipeOpened;
	private: int exitStatus = 0;

	public: Process(std::string const& command, std::vector<std::string> args = {})
	{
		for (Channel channel : this->allChannels) {
			if (pipe(this->pipes[channel].data())) {
				throw std::runtime_error("pipe creation failed");
			} else {
				this->pipeOpened[channel] = true;
			}
		}

		try {
			this->pid = Process::createSubProcess(std::bind(std::mem_fn(&Process::initChild), this, command, args));
		} catch (std::runtime_error const& e) {
			for (Channel channel : this->allChannels) {
				close(this->pipes[channel][PIPE_INDEX_READ]);
				close(this->pipes[channel][PIPE_INDEX_WRITE]);
			}

			throw e;
		}

		// close pipes which should only be written by the child process
		close(this->pipes[Channel::STDIN][PIPE_INDEX_READ]);
		close(this->pipes[Channel::STDOUT][PIPE_INDEX_WRITE]);
		close(this->pipes[Channel::STDERR][PIPE_INDEX_WRITE]);
	}

	public: ~Process()
	{
		// close remaining pipes
		this->closeAllPipes();
	}

	public: bool read(char& dest, Channel channel = Channel::STDOUT)
	{
		assert(this->pipeOpened[channel]);

		char c;
		int readCount = ::read(this->pipes[channel][PIPE_INDEX_READ], &c, 1);

		if (readCount == 0) {
			return false;
		}

		dest = char(c);
		return true;
	}

	public: void write(std::string const& data)
	{
		assert(this->pipeOpened[Channel::STDIN]);

		::write(this->pipes[Channel::STDIN][PIPE_INDEX_WRITE], data.c_str(), data.length());
	}

	public: void write(char c)
	{
		this->write(std::string() + c);
	}

	public: void closeWritePipe()
	{
		assert(this->pipeOpened[Channel::STDIN]);

		close(this->pipes[Channel::STDIN][PIPE_INDEX_WRITE]);
		this->pipeOpened[Channel::STDIN] = false;
	}

	public: int finish()
	{
		this->closeAllPipes();

	    waitpid(this->pid, &this->exitStatus, WNOHANG);

		return this->exitStatus;
	}

	public: void kill(int signal = SIGTERM)
	{
		::kill(this->pid, signal);
	}

	private: static pid_t createSubProcess(std::function<void ()> childInit)
	{
		pid_t pid = vfork();

		if (pid < (pid_t) 0) {
			throw std::runtime_error("fork failed");
		}

		if (pid == (pid_t) 0) {
			childInit(); // should exit
		}

		return pid;
	}

	/**
	 * run commands to initialize the child process
	 * this should be the only function running at child context
	 * because of vfork the actions should be limited to close, open and dup*
	 */
	private: void initChild(std::string const& command, std::vector<std::string> args) const
	{
		dup2(this->pipes.at(Channel::STDIN)[PIPE_INDEX_READ], STDIN_FILENO);
		dup2(this->pipes.at(Channel::STDOUT)[PIPE_INDEX_WRITE], STDOUT_FILENO);
		dup2(this->pipes.at(Channel::STDERR)[PIPE_INDEX_WRITE], STDERR_FILENO);

		for (Channel channel : this->allChannels) {
			close(this->pipes.at(channel)[PIPE_INDEX_READ]);
			close(this->pipes.at(channel)[PIPE_INDEX_WRITE]);
		}

		execvp(command.c_str(), this->buildArgList(command, args).data());
		std::cerr << "failed running command '" << command << "'" << std::endl;
		_exit(EXIT_FAILURE);
	}

	private: void closeAllPipes()
	{
		if (this->pipeOpened[Channel::STDIN]) {
			close(this->pipes[Channel::STDIN][PIPE_INDEX_WRITE]);
			this->pipeOpened[Channel::STDIN] = false;
		}
		if (this->pipeOpened[Channel::STDOUT]) {
			close(this->pipes[Channel::STDOUT][PIPE_INDEX_READ]);
			this->pipeOpened[Channel::STDOUT] = false;
		}
		if (this->pipeOpened[Channel::STDERR]) {
			close(this->pipes[Channel::STDERR][PIPE_INDEX_READ]);
			this->pipeOpened[Channel::STDERR] = false;
		}
	}

	private: std::vector<char*> buildArgList(std::string const& command, std::vector<std::string> const& source) const
	{
		std::vector<char*> result;
		result.reserve(source.size() + 2);
		result.push_back(const_cast<char*>(command.c_str())); // first argument is the command
		for (auto const& str : source) {
			result.push_back(const_cast<char*>(str.c_str()));
		}
		result.push_back(nullptr);

		return result;
	}
};
#include <thread>

int main(int argc, char** argv)
{
	Process bash("bash");

//	FILE* file = popen("/home/daniel/Schreibtisch/test3.sh", "r");
//	int c;
//	while ((c = fgetc(file)) != EOF) {
//		//std::cout << char(c);
//	}
//	std::cout << "exit status: " << pclose(file) << std::endl;
//	return 0;

//	Process p("echo", {"test", "blubb"});
//
	std::function<void(Process::Channel)> readChannel = [&bash] (Process::Channel ch) {
		char nextChar;
		while (bash.read(nextChar, ch)) {
			if (ch == Process::Channel::STDERR) {
				std::cerr << nextChar;
			} else {
				std::cout << nextChar;
			}
		}
	};

	std::thread t1(readChannel, Process::Channel::STDOUT);
	std::thread t2(readChannel, Process::Channel::STDERR);

	bash.write("echo 'du -sh /usr/*'\n");
	bash.write("du -sh /usr/*\n");
	bash.write("echo 'echo blabla'\n");
	bash.write("echo blabla\n");
	bash.write("echo myerror 1>&2\n");
	bash.closeWritePipe();
//	bash.write('\0');

//	sleep(2);
//	std::cout << "_____ now killing ______" << std::endl;
//	p.kill();

	t1.join();
	t2.join();

	bash.kill();

//	std::cout << "_____ threads closed ______" << std::endl;

	std::cout << std::endl << "process finished with status " << bash.finish() << std::endl;

	return 0;

	Gtk::Main app(argc, argv);

	Glib::thread_init();

	Gtk::Window win;
	Gtk::VBox vbMain;
	Gtk::ScrolledWindow scr;
	Gtk::TextView tv;
	Gtk::Button btt("run!");

	btt.signal_clicked().connect(sigc::bind<Gtk::TextView&>(sigc::ptr_fun(execCommand), tv));

	scr.add(tv);

	vbMain.pack_start(btt, Gtk::PACK_SHRINK);
	vbMain.pack_start(scr);

	win.add(vbMain);

	win.set_default_size(400, 300);

	win.show_all();

	app.run(win);
}

