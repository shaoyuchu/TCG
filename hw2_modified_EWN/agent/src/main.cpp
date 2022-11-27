#include <iostream>

#include "board.hpp"
using namespace std;

void setInitialPosition(Board& board) {
    int redPos[6][2] = {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 0}};
    int bluePos[6][2] = {{3, 6}, {4, 5}, {4, 6}, {5, 4}, {5, 5}, {5, 6}};
    for (int i = 0; i < 6; i++)
        board.setCube(redPos[i][0], redPos[i][1], Cube(Color::Red, getchar()));
    for (int i = 0; i < 6; i++)
        board.setCube(bluePos[i][0], bluePos[i][1], Cube(Color::Blue, getchar()));
}

void setPlayer(Board& board) {
    // always play as the Red
    char player = getchar();
    if (player == 'f') {
        board.setNextTurn(Color::Red);
    } else if (player == 'b') {
        // flip the board
        board.flip();
        board.setNextTurn(Color::Blue);
    }
}

int main() {
    Board initialBoard;
    setInitialPosition(initialBoard);
    setPlayer(initialBoard);

    // test code
    cerr << initialBoard << endl;
    vector<Ply> validPlys = initialBoard.getNextPly();
    for (Ply ply : validPlys) {
        cerr << ply << endl;
    }

    return 0;
}