#pragma once
#include <iomanip>
#include <iostream>
#include <string>

#include "board/board.h"
using namespace std;

istream& operator>>(istream& in, Board* board) {
    int buf = 0;
    for (int i = 0; i < M * N; i++) {
        in >> buf;
        board->set(i, buf);
    }
    return in;
}

ostream& operator<<(ostream& out, Board* board) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            out << setw(4) << board->get(i * N + j);
        }
        out << '\n';
    }
    return out;
}

string pattern2Str(vector<int>& pattern) {
    string result;
    for (int i = 0; i < pattern.size(); i++) {
        result += (to_string(pattern[i]) + "-");
    }
    result.erase(result.size() - 1);
    return result;
}