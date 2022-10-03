#pragma once
#include <iomanip>
#include <iostream>

#include "board.h"
using namespace std;

istream& operator>>(istream& in, Board& board) {
    for (int i = 0; i < M * N; i++) {
        in >> board(i);
    }

    return in;
}

ostream& operator<<(ostream& out, Board& board) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            out << setw(4) << board(i * N + j);
        }
        out << '\n';
    }
    return out;
}
