/*! \file computer.hpp
    \brief computer.hpp
    \author agent for playing the game
    \course Theory of Computer Game (TCG)
*/
#ifndef AGENT_HPP
#define AGENT_HPP
// POSIX
#include <unistd.h>
#include <signal.h> // SIG
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
// C++
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cassert>
// std
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include <chrono>
#include <algorithm>
// einstein
#include "einstein.hpp"

// clear n lines
#define CLR_LINES(n) do{\
for(int cnt=(n); cnt--;){\
std::cout << "\033[1F\033[2K";\
}\
}while(0)

class _com_io {
private:
	static bool init;
	static void catch_sigpipe ( int sig ) {}
	mutable int pid = -1;
	int rfd, wfd; // read/write fd;
public:
	_com_io ( _com_io const& ) = default;
	// don't allow = operator to copy
	_com_io &operator = ( _com_io const & ) = delete;
	_com_io &operator = ( _com_io const &&a ) {
		if ( pid != a.pid ) {
			suicide();
			pid = a.pid;
			rfd = a.rfd, wfd = a.wfd;
			a.pid = -1; // remove process from pointer a
		}
		return (*this);
	}
	_com_io ( _com_io && a ) {
		*this = std::move(a);
	}
	~_com_io () {
		suicide();
	}
	explicit _com_io ( std::string const &exec_name ) {
		if ( init ) { // initialization
			init = 0;
			struct sigaction sig_catcher;
			sig_catcher.sa_handler = catch_sigpipe;
			sig_catcher.sa_flags = 0;
			if ( sigemptyset(&sig_catcher.sa_mask) == -1 ) {
				throw std::runtime_error("AGENT(exec_name)::sigemptyset error\n");
			}
			if ( sigaction(SIGPIPE, &sig_catcher, nullptr) == -1 ) {
				throw std::runtime_error("AGENT(exec_name)::sigaction error\n");
			}
		}
		// double-end pipe
		int _rfd[2], _wfd[2];
		if ( pipe(_rfd)==-1 or pipe(_wfd)==-1 ) {
			throw std::runtime_error("AGENT(exec_name)::pipe() error\n");
		}
		int _pid = fork();
		if ( _pid == -1 ) {
			throw std::runtime_error("AGENT(exec_name)::fork() error\n");
		}
		// 0 = stdin, 1 = stdout
		if ( !_pid ) { // child process
		// child reads from wfd[0] writes to rfd[1]
			if ( close(_rfd[0])==-1 or close(_wfd[1])==-1 ) {
				throw std::runtime_error("AGENT(exec_name)::child close() error\n");
			}
			// give to stdin/stdout
			if ( dup2(_wfd[0], 0)==-1 or dup2(_rfd[1], 1)==-1 ) {
				throw std::runtime_error("AGENT(exec_name)::child dup2() error\n");
			}
			if ( close(_rfd[1])==-1 or close(_wfd[0])==-1 ) {
				throw std::runtime_error("AGENT(exec_name)::child close() error - 1\n");
			}
			if ( execlp(exec_name.c_str(), nullptr) == -1 ) {
				throw std::runtime_error("AGENT(exec_name)::child execlp() error\n");
			}
		}
		// parent process (judge)
		// parent reads from rfd[0], writes to wfd[1]
		if ( close(_rfd[1])==-1 or close(_wfd[0])==-1 ) {
			throw std::runtime_error("AGENT(exec_name)::parent close() error\n");
		}
		pid = _pid;
		rfd = _rfd[0];
		wfd = _wfd[1];
		//std::cerr << "pid: " << pid << std::endl;
	}
	// -1 if fail
	// does not allow over estimated sz
	std::string recv ( std::size_t sz, int timeout_sec ) const {
		//int done = 0;
		if ( pid == -1 ) {
			return ("");
		}
		std::string res;
		timeout_sec = std::max(timeout_sec, 0);
		int available = timeout_sec*1e6+1e5;
		struct timeval tv;
		tv.tv_sec = timeout_sec, tv.tv_usec = 0;

		std::vector<char> buf(sz+1);
		while ( sz ) {
#ifdef LINUX
			::fd_set fds;
#else
			struct fd_set fds;
#endif

			FD_ZERO(&fds);
			FD_SET(rfd, &fds);

			auto t0 = std::chrono::steady_clock::now();
			if ( select(rfd+1, &fds, nullptr, nullptr, &tv)==-1 ) {
				throw std::runtime_error("AGENT::recv::select() error\n");
			}
			auto t1 = std::chrono::steady_clock::now();
			int elapsed = 1e6*std::chrono::duration_cast<std::chrono::duration<double>>
				(t1-t0).count();
			if ( available-elapsed < 1e5 ) {
				return ("");
			}
			int buf_read = read(rfd, buf.data(), sz);
			switch ( buf_read ) {
				case -1 :
					throw std::runtime_error("AGENT::recv::read() error\n");
				case 0 :
					return ("");
				default :
					buf[buf_read] = 0;
					res += buf.data();
					std::fill(buf.data(), buf.data()+buf_read, 0);
					sz -= buf_read;
			}
		}
		return (res);
	}
	// return (false) when write fail
	bool send ( char c ) const {
		if ( pid == -1 ) {
			return (0);
		}
		int buf_write = write(wfd, &c, 1);
		return (buf_write);
	}
	// return (false) when write fail
	bool send ( std::string str ) const {
		if ( pid == -1 ) {
			return (0);
		}
		unsigned int offset = 0;
		while ( offset < str.size() ) {
			int buf_write  = write(wfd, str.c_str()+offset, str.size()-offset);
			if ( buf_write <= 0 ) {
				return (0);
			}
			offset += buf_write;
		}
		return (1);
	}
	// kill self (allow timeout before kill)
	void suicide ( int timeout_sec=0 ) {
		if ( pid == -1 ) {
			return ;
		}
		if ( timeout_sec > 0 ) {
			sleep(timeout_sec);
		}
		if ( close(rfd)==-1 or close(wfd)==-1 ) {
			throw std::runtime_error("AGENT::suicide::close() error\n");
		}
		kill(pid, SIGKILL);
		if ( waitpid(pid, nullptr, 0) == -1 ) {
			throw std::runtime_error("AGENT::suicide::waitpid() error\n");
		}
		std::cerr << "killed " << pid << std::endl;
		pid = -1;
	}
};
bool _com_io::init = 1;
std::string do_human_move ( BOARD_GUI &b, bool undo_twice ) {
	CLR_LINES(4);
	std::cout << "\033[1;33mChange: Arrow Keys\033[m\n";
	std::cout << "\033[1;33mConfirm: Z\033[m\n";
	std::cout << "\033[1;33mUndo: U\033[m\n\n";

	auto ml = b.move_list();
	if ( ml.at(0).first == 15 ) { // no available move
		b.do_move(15, 15);
		return ("??");
	}
	Color color = b.turn();
	int num = 16, dir = 16; // {16, 16} = pass
	b.no_hl = 0; // open highlight for GUI
	b.set_hl(color, ml[0].first);
	b.set_dir(ml[0].second);
	std::cout << b << "\n\n";
	
	int now_move = 0;
	bool stay = 1;
	while ( stay ) {
		char c = getch();
		switch ( c ) {
			case 'Z': case 'z':
				stay = 0;
				b.do_move(ml[now_move].first, ml[now_move].second);
				if ( b.winner() != Color::OTHER ) { b.no_hl = 1; }
				num = ml[now_move].first; dir = ml[now_move].second;
				break;
			case 'U': case 'u':
				if ( b.history.size()<3 and undo_twice ) { std::cout << "\a"; continue; }
				stay = 0;
				b.undo_move();
				if ( undo_twice ) { // undo twice
					b.undo_move();
				}
				break;
			case '\033':
				c = getch();
				if ( c != '[' ) {
					std::cout << "\a"; continue;
				}
				c = getch();
				if ( !strchr("ABCD", c) ) {
					std::cout << "\a"; continue;
				}
				now_move = (now_move+((c=='B'||c=='C')? 1: ml.size()-1))%ml.size();
				b.set_hl(color, ml[now_move].first);
				b.set_dir(ml[now_move].second);
				break;
			default: std::cout << "\a"; continue;
		}
		// change
		CLR_LINES(BOARD_SZ-1+6);
		std::cout << b << "\n\n";
	}
	std::string res;
	res += (char)(num+'0'); 
	res += (char)(dir+'0');

	return (res);
}

struct _agent { 
	using IO = _com_io;
	using PII = std::pair<int, int>;
	using PSS = std::pair<std::string, std::string>;

	bool undo_twice = 0;
	bool my_turn = 0;
	BOARD_GUI *b;
	IO *io = nullptr;
	int time_limit = 10;
	_agent () noexcept {
	}
	_agent ( std::string exec_name, int limit=10 ) noexcept {
		time_limit = limit;
		io = new IO(exec_name);
	}

// initialize agent
	void init ( PSS p, char start ) noexcept {
		if ( io != nullptr ) {
			io->send(p.first);
			io->send(p.second);
			io->send(start);
		} else {
			//if ( b != nullptr ) { delete b; }// TODO: destructors in einstein.hpp
			b = new BOARD_GUI(p.first, p.second);
			my_turn = (start=='f');
		}
	}
// tell restart
	void restart ( char c ) noexcept {
		if ( io != nullptr ) {
			io->send(c);
		}
	}
// give opponent move to agent
	void give_move ( std::string m ) noexcept {
		if ( io != nullptr ) {	
			io->send(m);
		} else {
			b->do_move(m[0]-'0', m[1]-'0');
		}
	}
// get move of the agent
	std::string get_move () {
		if ( io != nullptr ) {
			return (io->recv(2, time_limit));
		} else {
			return (do_human_move(*(b), undo_twice));
		}
	}
};
using AGENT = _agent;
#endif

