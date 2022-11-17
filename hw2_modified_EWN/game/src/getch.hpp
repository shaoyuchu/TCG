/*! \file getch.hpp
    \brief gain terminal keystroke in Linux
    \author Yung-Hsien Chung (hiiragi4000)
    \course Theory of Computer Game (TCG)
*/
#ifndef GETCH_HPP
#define GETCH_HPP
// C
#include <termios.h>
// C++
#include <cstdio>

static struct termios old, neww;

// Initialize new terminal i/o settings
static void initTermios(int echo){
    tcgetattr(0, &old); // grab old terminal i/o settings
    neww = old; // make new settings same as old settings
    neww.c_lflag &= ~ICANON; // disable buffered i/o
    if(echo){
        neww.c_lflag |= ECHO; // set echo mode
    }else{
        neww.c_lflag &= ~ECHO; // set no echo mode
    }
    tcsetattr(0, TCSANOW, &neww); // use these new terminal i/o settings now
}

// Restore old terminal i/o settings
static void resetTermios(){
    tcsetattr(0, TCSANOW, &old);
}
// return the character read after a keystroke immediately
// similar to the 'getch()' defined in conio.h on Windows
// Read 1 character - echo defines echo mode
static char getch_(int echo){
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return (ch);
}

// Read 1 character without echo
char getch(){
    return (getch_(0));
}

// Read 1 character with echo
char getche(){
    return (getch_(1));
}
#endif
