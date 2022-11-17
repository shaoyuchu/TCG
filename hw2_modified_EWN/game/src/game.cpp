/*! \file game.cpp
	\brief parses the arguments (console mode is disabled)
	\author Yueh-Ting Chen (eopXD)
	\course Theory of Computer Game (TCG)
*/

#include "play.hpp"
#include "getch.hpp"

#include <cstring>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <fstream>

std::fstream flog;

double FPS = 0.05;

#define EINSTEIN_WRONG_ARG do {\
fprintf(stdout, "Usage: ./game [-r round_num] [-l logfile_name] [-p0 exec_name0] [-p1 exec_name1] [-fps] [-gui]\n");\
fprintf(stdout, " -r [round_num] (default 10)\n");\
fprintf(stdout, " -l [logfile_name] (default \'log\')\n");\
fprintf(stdout, " -p0 [exec_player0] (default human)\n");\
fprintf(stdout, " -p1 [exec_player1] (default human)\n");\
fprintf(stdout, " -fps [frame_per_sec] (default 20)\n");\
fprintf(stdout, " -gui [0/1] (default activated)\n");\
return (1);\
}while(0)

bool can_exec ( const char *file ) {
//https://stackoverflow.com/questions/5719694/how-to-check-if-file-is-executable-in-c
	struct stat st;
	if ( stat(file, &st) < 0 ) { return (false); }
	if ( (st.st_mode & S_IEXEC) != 0 ) { return (true); }
	return (false);
}
int main ( int argc, char **argv )
{
	int arg_r = 10, arg_gui = 1, arg_fps = 20;
	char logfile[105] = ".log.game";
	char arg_p0[105] = "", arg_p1[105] = "";
	for ( int i=1; i<argc; ++i ) {
		if ( strcmp(argv[i], "-r") == 0 ) {
			arg_r = atoi(argv[++i]);
		} else if ( strcmp(argv[i], "-fps") == 0 ) {
			arg_fps = atoi(argv[++i]);
		} else if ( strcmp(argv[i], "-gui") == 0 ) {
			arg_gui = atoi(argv[++i]);
		} else if ( strcmp(argv[i], "-l") == 0 ) {
			strcpy(logfile, argv[++i]);
		} else if ( strcmp(argv[i], "-p0") == 0 ) {
			strcpy(arg_p0, argv[++i]);
		} else if ( strcmp(argv[i], "-p1") == 0 ) {
			strcpy(arg_p1, argv[++i]);
		} else {
			EINSTEIN_WRONG_ARG;
		}
	}
	if ( arg_r<=0 or arg_gui<0 or arg_fps<0 ) { EINSTEIN_WRONG_ARG; }
	if ( strlen(arg_p0)>0 and !can_exec(arg_p0) ) { 
		throw std::runtime_error("specified p0 is not an executable\n");
	}
	if ( strlen(arg_p1)>0 and !can_exec(arg_p1) ) {
		throw std::runtime_error("specified p1 is not an executable\n");
	}
	flog.open(logfile, std::fstream::out);
	if ( !flog.is_open() ) { 
		throw std::runtime_error("error opening logfile\n"); 
	}
	FPS = (arg_fps==0)?0: 1.0/arg_fps;
	if ( arg_fps == 0 ) { 
		arg_gui = 0; 
	}
	if ( arg_gui == 0 ){
		FPS = 0;
	}
	
	general(arg_p0, arg_p1, arg_r, arg_gui);

/* Calls console... */
/*
	CLR_SCREEN;
	std::cout << "\033[1;36mWelcome to Einstein Würfelt Nicht! (Kari) " << BOARD_SZ << "x" << BOARD_SZ << "\033[m\n\n";
	std::cout << "\033[1;36m[0] Human vs. Human" << "\033[m\n";
	std::cout << "\033[1;36m[1] Human vs. AI" << "\033[m\n";
	std::cout << "\033[1;36m[2] AI    vs. AI" << "\033[m\n\n\n";
	int round;
	std::string exec_name0, exec_name1;
	bool stay = 1;
	while ( stay ){
	char c = getch();
	switch ( c ) {
		case '0': 
			general("", "");
			stay = 0; break;
		case '1':
			std::cout << "\033[1;33mInsert 0th executable (ex: \'./exec\'): \033[m";
			std::cin >> exec_name0;
			general(exec_name0, "");
			stay = 0; break;
		case '2':
			std::cout << "\033[1;33mInsert 0th executable (ex: \'./exec\'): \033[m";
			std::cin >> exec_name0;
			std::cout << "\033[1;33mInsert 1st executable (ex: \'./exec\'): \033[m";
			std::cin >> exec_name1;
			std::cout << "\033[1;33mHow many round(s) do you want to play? \033[m";
			std::cin >> round;
			general(exec_name0, exec_name1, round);
			stay = 0; break;
		default: 
			std::cout << "\a"; continue;
	}}
*/
	return (0);
}
