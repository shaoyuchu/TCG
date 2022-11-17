/*! \file play.hpp
    \brief play.hpp
    \author makes the game playable
    \course Theory of Computer Game (TCG)
*/
#ifndef PLAY_HPP
#define PLAY_HPP

#include <unistd.h>

#include <fstream>
#include <string>
#include <utility>

#include "getch.hpp"
#include "einstein.hpp"

#include "agent.hpp"

extern double FPS; // frame per second

#define F first
#define S second
using PII = std::pair<int, int>;
using PSS = std::pair<std::string, std::string>;

// clear n lines
#define CLR_LINES(n) do{\
for(int cnt=(n); cnt--;){\
std::cout << "\033[1F\033[2K";\
}\
}while(0)
#define CLR_SCREEN std::cout << "\033[2J\033[1;1H"

extern std::fstream flog;

void announce_winner ( Color winner, int who_first, int *win_count, bool gui ) {
	if ( winner == Color::R ) {
		if ( gui ) {
			std::cout << "\033[31mRed Player Wins!!!\033[m\n\n";
		}
		++win_count[who_first];
	}
	else if ( winner == Color::B ) {
		if ( gui ) {
			std::cout << "\033[34mBlue Player Wins!!!\033[m\n\n";
		}
		++win_count[!who_first];
	} else {
		if ( gui ) {
			std::cout << "\033[32mDraw ~ OAO\033[m\n\n";
		}
		++win_count[2];
	}
}
// (new_board, restart)
PII ending () {
	std::cout << "\033[1;36mRestart ?" << "\033[m\n";
	std::cout << "\033[1;36m(Z) Same Board" << "\033[m\n";
	std::cout << "\033[1;36m(Y) New Board" << "\033[m\n";
	std::cout << "\033[1;36m(N) Leave" << "\033[m\n\n";
	bool stay = 1;
	while ( stay ){
	char c = getch();
	switch ( c ) {
		case 'Z': case 'z':
			return (std::make_pair(0, 1));
		case 'Y': case 'y':
			return (std::make_pair(1, 1));
		case 'N': case 'n':
			return (std::make_pair(0, 0));
		default: 
			std::cout << "\a"; continue;
	}}
	return (std::make_pair(0, 0));
}
void flip_bit ( bool &x ) { x = !x; }
void header ( bool who_start, int *win_count ) {
	// change
	std::cout << "\033[1;36mEinstein Würfelt Nicht! (Kari) " << BOARD_SZ-1 << "x" << BOARD_SZ << "\033[m\n\n";
	std::cout << (who_start==0?"\033[31m":"\033[34m") << "Player 0: " << win_count[0] << "\033[m\n";
	std::cout << (who_start==1?"\033[31m":"\033[34m") << "Player 1: " << win_count[1] << "\033[m\n";
	std::cout << "\033[32mDraw: " << win_count[2] << "\033[m\n\n\n\n\n\n";
}
void final_score ( int *win_count ) {
	std::cout << "\n\033[1;32mFinal Score:\n";
	std::cout << "\033[1;32mPlayer 0: " << win_count[0] << "\033[m\n";
	std::cout << "\033[1;32mPlayer 1: " << win_count[1] << "\033[m\n";
	std::cout << "\033[1;32mDraw: " << win_count[2] << "\033[m\n";
}
void general ( std::string exec_name0, std::string exec_name1, int round, bool gui ) {
	AGENT *player[2];
	if ( exec_name0.size() == 0 ) {
		player[0] = new AGENT();
	} else {
		player[0] = new AGENT(exec_name0);
		player[0]->undo_twice = (exec_name1.size() == 0);
	}
	if ( exec_name1.size() == 0 ) {
		player[1] = new AGENT();
		player[1]->undo_twice = (exec_name0.size() != 0);
	} else {
		player[1] = new AGENT(exec_name1);
	}
	
	bool who_start = 0;
	int round_cnt = 1;
	int win_count[3] = {};
	BOARD_GUI prev;
	bool restart = 1, new_board = 1;
	while ( restart and round>0 ) {
		flog << "round:" << round_cnt++ << std::endl;
		BOARD_GUI b;
		if ( !new_board ) { 
			b = prev; 
		}
		prev = b;
		b.no_hl = 1;

		PSS init_pos = b.give_init_position();
		player[0]->init(init_pos, (who_start==0)?'f':'s');
		player[1]->init(init_pos, (who_start==1)?'f':'s');
		flog << "init:" << init_pos.F << init_pos.S << std::endl;
// start playing
	// display
		CLR_SCREEN;
		if ( gui ) {
			header(who_start, win_count);
			std::cout << b << "\n\n";
		}
		usleep(FPS*1000000);
		if ( gui ) {
			// chaneg
			CLR_LINES(BOARD_SZ-1+6);
		}
		bool now_turn = who_start;
		flog << "turn:" << who_start << std::endl;
		while ( b.winner() == Color::OTHER ) {
			std::string m = player[now_turn]->get_move();
			flog << now_turn << m << std::endl;
			b.do_move(m[0]-'0', m[1]-'0');
			player[!now_turn]->give_move(m);
			if ( exec_name0.size()!=0 and m[0]-'0'==16 ) { // undo twice when Player vs. AI
				b.do_move(m[0]-'0', m[1]-'0');
				flip_bit(now_turn);
			}
			flip_bit(now_turn);
	// display
			if ( gui ) {
				CLR_SCREEN;
				header(who_start, win_count);
				std::cout << b << "\n\n";
			}
			usleep(FPS*1000000);
			// change
			if ( b.winner() == Color::OTHER ) { CLR_LINES(BOARD_SZ-1+6); }
		}
		announce_winner(b.winner(), who_start, win_count, gui);
		
		flog << "winner:" << b.winner() << std::endl;
		if ( round == 1 ) { break; }
		if ( exec_name1.size() ==0 ) {
			PII p = ending();
			new_board = p.first, restart = p.second;
			player[0]->restart(restart?'y':'n');
			player[1]->restart(restart?'y':'n');
		}else {
			player[0]->restart((round==1)?'n':'y');
			player[1]->restart((round==1)?'n':'y');
		}
		flip_bit(who_start);
		round--;
	}
	final_score(win_count);
}

#endif

