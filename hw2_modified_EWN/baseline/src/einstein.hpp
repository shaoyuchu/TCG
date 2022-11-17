// Copyright (C) 2019 Yueh-Ting Chen (eopXD)
/*! \file einstein.hpp
	\brief structures of the game, you may want to know the APIs
	\author Yueh-Ting Chen (eopXD)
	\course Theory of Computer Game (TCG)
	\modified by kjchan (2022)
*/
#ifndef EINSTEIN_HPP
#define EINSTEIN_HPP

// C++
#include <cstdio>
#include <cstdint>
#include <cassert>
// std::
#include <iostream> // cin, cout, cerr
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <random> // default_random_engine
#include <chrono> // system_clock
#include <algorithm> // shuffle

extern std::fstream flog;

/* Color */
enum class Color { R, B, OTHER, NO_ONE };
// cast a scoped enum to its underlying type
template<typename E> typename std::underlying_type<E>::type constexpr enum2int ( E enumerator ) noexcept {
    return (static_cast<typename std::underlying_type<E>::type>(enumerator));
}
std::ostream &operator << ( std::ostream &os, Color const &c ) {
    return (os << "rb_x"[enum2int(c)]);
}

/* Cube */

#ifdef SEVEN
const int NUM_CUBE = 10; // 7x7
#else
const int NUM_CUBE = 6;
#endif

struct _cube {
	Color c = Color::OTHER;
	int num = NUM_CUBE;
	_cube () noexcept = default;
	_cube ( Color _c, int _num=0 ) noexcept {
		c = _c;
		num = _num;
	}
	_cube &operator = ( _cube const &cube ) {
		c = cube.c;
		num = cube.num;
		return (*this);

	}
	explicit operator bool () const noexcept {
		return (num==NUM_CUBE or c==Color::OTHER);
	}
	bool operator == ( _cube const &rhs ) const noexcept {
		return (c==rhs.c and num==rhs.num);
	}
	bool operator == ( _cube *const &rhs ) const noexcept {
		return (c==rhs->c and num==rhs->num);
	}
	bool operator != ( _cube const &rhs ) const noexcept {
		return (c!=rhs.c or num!=rhs.num);
	}
};
using CUBE = _cube;
std::ostream &operator << ( std::ostream &os, CUBE const &c ) {
	if ( c.c == Color::OTHER ) { return (os << "--"); }
	os << c.c;
	if ( c.num == 10 ) { return (os << "--"); }
	else { return (os << c.num); }
}

#ifdef SEVEN
const int NUM_POSITION = 49; // 7x7
const int BOARD_SZ = 7; // 7x7
#else
const int NUM_POSITION = 42;
const int BOARD_SZ = 7; // 6x7 in 2022 hw2
#endif

struct _square {
	CUBE *c = nullptr; // a square may/maynot be occupied by cube
	int pos = NUM_POSITION;
	_square () noexcept = default;
	_square ( int const &x, int const &y, Color _c, int _num ) noexcept {
		pos = x*BOARD_SZ+y;
		c = new CUBE(_c, _num);
	}
	_square ( int const &x, int const &y, CUBE *c_ptr=nullptr ) noexcept {
		pos = x*BOARD_SZ+y;
		c = c_ptr;
	}
	_square &operator = ( _square const &sq ) noexcept {
		c = sq.c;
		pos = sq.pos;
		return (*this);
	}
	bool operator == ( _square const &rhs ) const noexcept {
		return (c==rhs.c and pos==rhs.pos);
	}

	explicit operator bool () const noexcept {
		return (pos != NUM_POSITION);
	}
	int x () const noexcept { return (pos/BOARD_SZ); }
	int y () const noexcept { return (pos%BOARD_SZ); }
};
using SQUARE = _square; 
std::ostream &operator << ( std::ostream &os, SQUARE const &sq ) {
	if ( sq ) {
		os << "(" << sq.x() << ", " << sq.y() << ", ";
		if ( sq.c != nullptr ) {
			return (os << sq.c->c << ", " << sq.c->num << ")");
		}
		else {
			return (os << "X, X)");
		}
	}
	else {
		return (os << "(X, X, X, X)");
	}
}
const int NUM_PLAYER = 2;
const int R_PLAYER = 0;
const int B_PLAYER = 1;

#ifdef SEVEN
// 7x7 initial positions
const std::array<int, NUM_CUBE> init_cube_pos[2] = {
	{ 0,  1,  2,  3,  7,  8,  9, 14, 15, 21}, 
	{27, 33, 34, 39, 40, 41, 45, 46, 47, 48}};
std::array<int, NUM_CUBE> init_cube[2] = {
	{ 0,  1,  3,  6,  2,  4,  7,  5,  8,  9},
	{ 9,  8,  5,  7,  4,  2,  6,  3,  1 , 0}};
const int R_CORNER = 0;
const int B_CORNER = 48;
#else
// 6x7 initial position
// change
const std::array<int, NUM_CUBE> init_cube_pos[2] = {
	{ 0,  1,  2,  7,  8, 14},
	{27, 33, 34, 39, 40, 41}};
std::array<int, NUM_CUBE> init_cube[2] = {
	{ 0,  1,  3,  2,  4,  5},
	{ 5,  4,  2,  3,  1,  0}};
const int R_CORNER = 0;
const int B_CORNER = 41;
#endif

// [0] = Player0, [1] = Player1
const int dx[2][3] = {{1, 0, 1}, {-1, 0, -1}};
const int dy[2][3] = {{0, 1, 1}, {0, -1, -1}};

/* need move structure to record history */
struct _move {
	Color whose_move; // who moved this move
	CUBE *c = nullptr;
	int start_pos, end_pos; // encoded position - [0 ~ (NUM_POS-1)]
	bool eat_cube = 0; // if eat_cube = 1, then c != nullptr
	bool pass; // whether it passed the turn or not
	
	_move () noexcept = delete;
	_move ( Color who, int _s, int _e, int _eat=0, CUBE *_c=nullptr ) {
		if ( _eat==1 and _c==nullptr ) {
			throw std::runtime_error(
			 "_move constructor fail, eat but no CUBE pointer specified\n");	
		}
		whose_move = who;
		start_pos = _s, end_pos = _e;
		eat_cube = _eat;
		c = _c;
		pass = 0;
	}
	_move ( Color who ) {
		pass = 1;
	}
};
using MOVE = _move;
using VMOVE = std::vector<MOVE>;
struct _board {
	using ULL = unsigned long long;
	using PII = std::pair<int, int>;
	using PSS = std::pair<std::string, std::string>;
	using VII = std::vector<PII>;

	ULL seed;
	SQUARE initial[NUM_POSITION]; // save initial state for restart
	SQUARE now[NUM_POSITION];
	
	Color _winner = Color::OTHER; // OTHER = game winner not determined
	bool _turn = 0; // R moves first
	int turn_cnt = 1; // odd/even turn move odd/even pieces(start from odd)
	int num_cubes[2] = {NUM_CUBE, NUM_CUBE}; // both start with num_cubes cubes

	VMOVE history;

	_board ( ULL const &_seed=std::chrono::system_clock::now().time_since_epoch().count() )  
noexcept {
// generate initial cubes
		seed = _seed;
		for ( int i=0; i<NUM_PLAYER; ++i ) {
			#ifdef INIT_RANDOM_SHUFFLE // shuffle initial position
			std::shuffle(init_cube[i].begin(), init_cube[i].end(), 
				std::default_random_engine(seed+i));
			#endif
			for ( int j=0; j<NUM_CUBE; ++j ) {
				int x = init_cube_pos[i][j]/BOARD_SZ;
				int y = init_cube_pos[i][j]%BOARD_SZ;
				initial[init_cube_pos[i][j]] 
				 = SQUARE(x, y, static_cast<Color>(i), init_cube[i][j]);
				now[init_cube_pos[i][j]] 
				 = SQUARE(x, y, static_cast<Color>(i), init_cube[i][j]);
			}
		}
		history.clear();
	}
	_board ( std::string top_left, std::string bottom_right ) noexcept {
		assert(top_left.size()==NUM_CUBE and bottom_right.size()==NUM_CUBE);
		std::string str[2]; str[0] = top_left, str[1] = bottom_right;
		for ( int i=0; i<NUM_PLAYER; ++i ) {
			for ( int j=0; j<NUM_CUBE; ++j ) {
				int x = init_cube_pos[i][j]/BOARD_SZ;
				int y = init_cube_pos[i][j]%BOARD_SZ;
				initial[init_cube_pos[i][j]] 
				 = SQUARE(x, y, static_cast<Color>(i), str[i][j]-'0');
				now[init_cube_pos[i][j]] 
				 = SQUARE(x, y, static_cast<Color>(i), str[i][j]-'0');
			}
		}
	}
	_board ( _board const& ) = delete;

	_board &operator = ( _board const &b ) {
		seed = b.seed;
		for ( int i=0; i<NUM_POSITION; ++i ) {
			initial[i] = b.initial[i];
			now[i] = b.now[i];
		}
		_winner = b._winner;
		_turn = b._turn;
		turn_cnt = b.turn_cnt;
		num_cubes[0] = b.num_cubes[0], num_cubes[1] = b.num_cubes[1];
		for ( auto &m: b.history ) {
			history.push_back(m);
		}
		return (*this);
	}

	Color turn () const noexcept { return (static_cast<Color>(_turn)); }
	Color winner () const noexcept { return (_winner); }
	void next_turn () noexcept { turn_cnt += _turn; _turn = !_turn; }
	void prev_turn () noexcept { _turn = !_turn; turn_cnt -= _turn; }

	bool out ( int const &x, int const &y ) const noexcept {
		// change
		return (x<0 or y<0 or x>=BOARD_SZ-1 or y>=BOARD_SZ);
	}
	bool occupy ( int const &x, int const &y ) const noexcept {
		return (now[x*BOARD_SZ+y].c != nullptr);
	}
	// 0 = not over
	// 1 = player R wins, 2 = player B wins, 3 = draw
	int state () const noexcept {
		if ( num_cubes[1] == 0 ) { return (1); }
		if ( num_cubes[0] == 0 ) { return (2); }

		if ( now[R_CORNER].c!=nullptr and now[B_CORNER].c!=nullptr ) {
			//if ( now[0].c->c==Color::R and now[48].c->c==Color::R ) {
			//	return (1);
			//}
			//if ( now[0].c->c==Color::B and now[48].c->c==Color::B ) {
			//	return (2);
			//}

			if ( now[R_CORNER].c->c==Color::B and now[B_CORNER].c->c==Color::R ) {
				if ( now[R_CORNER].c->num < now[B_CORNER].c->num ) {
					return (2);
				}
				else if ( now[R_CORNER].c->num > now[B_CORNER].c->num ) {
					return (1);
				}
				else {
					return (3);
				}
			}
		}
		return (0);
	}
	void update_game ( int game_status ) noexcept {
		if ( game_status == 0 ) {
			_winner = Color::OTHER;
		}
		if ( game_status == 1 ) {
			_winner = Color::R;
		}
		if ( game_status == 2 ) {
			_winner = Color::B;
		}
		if ( game_status == 3 ) {
			_winner = Color::NO_ONE;
		}
	}
	PSS give_init_position () {
		std::string res[2];
		for ( int i=0; i<2; ++i ) {
			for ( int j=0; j<NUM_CUBE; ++j ) {
				res[i] += (char)(initial[init_cube_pos[i][j]].c->num+'0');
			}
		}
		return (make_pair(res[0], res[1]));
	}
	// return (-100, -100) if not found
	PII find ( Color who, int num ) const noexcept {
		CUBE x(who, num);
		for ( int i=0; i<NUM_POSITION; ++i ) {
			if ( now[i].c!=nullptr and x==now[i].c ) {
				return (std::make_pair(i/BOARD_SZ, i%BOARD_SZ));
			}
		}
		return (std::make_pair(-100, -100));
	}
	// vectors of (num, dir)
	VII move_list () const noexcept { // allow self eating
		if ( _winner != Color::OTHER ) {
			//std::cerr << "appear winner! " << turn() <<"\n";
			return {};
		}
		Color color = turn();
		int ply = enum2int(color);
		VII res;
		for ( int num=0; num<NUM_CUBE; ++num ) {
			// 6x6 rule: allow to move any piece
			// 7x7: move odd/even on odd/even turns
			#ifdef SEVEN 
			if ( (num%2) != (turn_cnt%2) ) {
				continue;
			}
			#endif

			PII pos = find(color, num);
			for ( int dir=0; dir<3; ++dir ) {
				int xx = pos.first+dx[ply][dir];
				int yy = pos.second+dy[ply][dir];
				// change
				if ( out(xx, yy) || yummy(num,dir)==-1 ) {
					continue;
				}
				res.emplace_back(num, dir);
			}
		}
		if ( res.empty() ) {
			res.emplace_back(15, 15);
		}
		return (res);
	}
	bool valid_move ( int const &num, int const &dir ) const noexcept {
		auto ml = move_list();
		return (std::find(ml.begin(), ml.end(), std::make_pair(num, dir)) != ml.end());
	}
	void undo_move () noexcept {
		if ( history.empty() ) {
			/* brand new board */
			return ;
		}
		MOVE m = history.back(); history.pop_back();
		if ( m.pass ) { // passes this turn
			prev_turn();
			return ;
		}

		now[m.start_pos].c = now[m.end_pos].c;
		now[m.end_pos].c = m.c;
		if ( m.eat_cube ) {
			++num_cubes[enum2int(m.c->c)];
		}
		update_game(state());
		prev_turn();
	}

	void do_move ( int const &num, int const &dir ) noexcept {
		Color color = turn();
		if ( num==15 and dir==15 ) { // no move for current player, pass
			history.emplace_back(color);
			next_turn(); return ;
		}
		if ( num==16 and dir==16 ) { // undo move
			undo_move(); return ;
		}
//		if ( !valid_move(num, dir) ) {
//			throw std::runtime_error("BOARD::do_move: game is over or the cube/direction is invalid");
//		}
		
		int ply = enum2int(color);
		PII pos = find(color, num);
		int xx = pos.first+dx[ply][dir];
		int yy = pos.second+dy[ply][dir];
		int now_pos = pos.first*BOARD_SZ+pos.second;
		int nxt_pos = xx*BOARD_SZ+yy;
// possible eat
		if ( occupy(xx, yy) ) {
			(now[nxt_pos].c->c == Color::R)?num_cubes[0]--:num_cubes[1]--;
			//delete now[nxt_pos].c;
			history.emplace_back(color, now_pos, nxt_pos, 1, now[nxt_pos].c);
		}
		else {
			history.emplace_back(color, now_pos, nxt_pos);
		}
// movement
		now[nxt_pos].c = now[now_pos].c;
		now[now_pos].c = nullptr;
// update game state
		update_game(state());
		next_turn();
	}
	int yummy ( int const &num, int const &dir ) const noexcept {
// return if this move eats a piece
// 1 = eat opponent, 0 = no eat, -1 = eat self piece
		if ( num == 15 or num == 16 ) {
			return (0);
		}
		Color color = turn();
		int ply = enum2int(color);
		PII pos = find(color, num);
		int xx = pos.first+dx[ply][dir];
		int yy = pos.second+dy[ply][dir];
		int nxt_pos = xx*BOARD_SZ+yy;
		if ( occupy(xx, yy) ) {
			if ( now[nxt_pos].c->c == color ) {
				return (-1);
			}
			else {
				return (1);
			}
		} else {
			return (0);
		}
	}
	int yummy ( PII &p ) const noexcept {
		return (yummy(p.first, p.second));
	}
	void do_move ( PII &p ) noexcept {
		do_move(p.first, p.second);
	}
	std::string send_move ( PII &p ) const noexcept {
		std::string m;
		m += (char)(p.first+'0');
		m += (char)(p.second+'0');
		return (m);
	}
};
using BOARD = _board;
std::ostream &operator << ( std::ostream &os, BOARD const &b ) {
	os << "seed: " << b.seed << "\n";
	os << "winner: " << b._winner << "\n";
	for ( int i=0; i<NUM_POSITION; ++i ) {
		os << b.now[i] << " \n"[(i%BOARD_SZ)==(BOARD_SZ-1)];
	}
	return (os);
}

// for display, need cureent position (highlighted and direction)
struct _board_gui : BOARD {
	SQUARE hl_pos = SQUARE(0, 0);
	int dir = 0;
	bool no_hl = 0;
	_board_gui () noexcept = default;
	_board_gui ( std::string a, std::string b) : BOARD(a, b) {}
	_board_gui ( int x, int y, int _dir ) noexcept {
		hl_pos = SQUARE(x, y);
		dir = _dir;
	}
	void set_hl ( Color c, int num ) {
		PII pos = find(c, num);
		hl_pos = SQUARE(pos.first, pos.second);
	}
	void set_dir ( int _dir ) {
		dir = _dir;
	}
};
using BOARD_GUI = _board_gui;
std::array<std::string, 10> const CUBE_ICON
{{"０", "１", "２", "３", "４", "５", "６", "７", "８", "９"}};
std::ostream &operator << ( std::ostream &os, BOARD_GUI const &b_gui ) {
	Color color = b_gui.turn();
	int who = enum2int(color);
	os << "\033[m"; // clear scheme
	os << "\033[1;36mTurn " << b_gui.turn_cnt << "\033[m\n\n";
	SQUARE hl_dir(b_gui.hl_pos.x()+dx[who][b_gui.dir], b_gui.hl_pos.y()+dy[who][b_gui.dir]);
//change
	for ( int i=0; i<BOARD_SZ-1; ++i ) {
		for ( int j=0; j<BOARD_SZ; ++j ) {
			int pos = i*BOARD_SZ+j;
			SQUARE ij = SQUARE(i, j);
			if ( !b_gui.no_hl and ij == hl_dir ) {
				if ( color == Color::R ) { os << "\033[1;33;46m＊"; }
				else { os << "\033[1;32;45m＊"; }
			}
			else {
				if ( b_gui.now[pos].c != nullptr ) {
					if ( b_gui.now[pos].c->c == Color::R ) {
						if ( !b_gui.no_hl and ij == b_gui.hl_pos ) { os << "\033[30;42m"; }
						else { os << "\033[1;41m"; }
					}
					else {
						if ( !b_gui.no_hl and ij == b_gui.hl_pos ) { os << "\033[30;43m"; }
						else { os << "\033[1;44m"; }
					}
					os << CUBE_ICON[b_gui.now[pos].c->num];
				}
				else {
					os << "\033[47m  ";
				}
			}
			os << "\033[m";
		}
		os << "\n";
	}
	os << "\033[31mR pieces: " << b_gui.num_cubes[0] << "\033[m\n";
	os << "\033[34mB pieces: " << b_gui.num_cubes[1] << "\033[m\n";
	return (os);
}

#endif
