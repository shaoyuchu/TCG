#pragma once
#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <vector>
#define N 5
#define M 4

/*
In this programing exercise, the goal board configuration is:
    1   2   3   4   5
    6   7   8  -1  10
   -1  12  -1  14  15
   16  17  18   0   0

-1: non-movable tile
 0: empty cell
your task is to find the least number of slides.

Definition for the MxN board,

N:  0  1  2  3  4
M:________________
0|   1  2  3  4  5
1|   6  7  8  9 10
2|  11 12 13 14 15
3|  16 17 18 19 20

1-d array style:
(0)==1 (1)==2 ... (5)==6 ... (19)==20
2-d array style:
[0][0]==1 [0][1]==2 ... [0][1]==6 ... [3][4]==20
*/

class Board {
   public:
    enum Action { up, down, left, right, null };

   public:
    Board() {}

    int& operator()(int i) { return puzzle[i / N][i % N]; }
    std::array<int, N>& operator[](int i) { return puzzle[i]; }

    /*
    slide example:
        1   2   3   4   5
        6   7   8  -1  10
       -1  12  -1   0  15
       16  17  18  14   0

    slide the tile number 14 up:
    1-d array style: slide(18,Action::up)
    2-d array style: slide(3,3,Action::up)
    */

    // 1-d array style
    void slide(int i, Action act) { slide(i / N, i % N, act); }

    // 2-d array style
    void slide(int i, int j, Action act) {
        bool invalid = false;

        if (puzzle[i][j] == 0 || puzzle[i][j] == -1) {
            std::cout << "Invalid slide" << '\n';
            exit(-1);
        }

        if (act == Action::up) {
            if (i - 1 >= 0 && puzzle[i - 1][j] == 0) {
                std::swap(puzzle[i][j], puzzle[i - 1][j]);
            } else
                invalid = true;
        } else if (act == Action::down) {
            if (i + 1 < M && puzzle[i + 1][j] == 0) {
                std::swap(puzzle[i][j], puzzle[i + 1][j]);
            } else
                invalid = true;
        } else if (act == Action::left) {
            if (j - 1 >= 0 && puzzle[i][j - 1] == 0) {
                std::swap(puzzle[i][j], puzzle[i][j - 1]);
            } else
                invalid = true;
        } else if (act == Action::right) {
            if (j + 1 < N && puzzle[i][j + 1] == 0) {
                std::swap(puzzle[i][j], puzzle[i][j + 1]);
            } else
                invalid = true;
        } else
            invalid = true;

        if (invalid) {
            std::cout << act << '\n';
            std::cout << "Invalid slide" << '\n';
            exit(-1);
        }
    }

    bool finish() {
        for (int i = 0; i < M * N; i++) {
            if (!(this->operator()(i) == i + 1 || this->operator()(i) == 0 ||
                  this->operator()(i) == -1))
                return false;
        }
        return true;
    }

   private:
    std::array<std::array<int, N>, M> puzzle = {0};
};

// Board b;
// cin >> b;
std::istream& operator>>(std::istream& in, Board& b) {
    for (int i = 0; i < M * N; i++) {
        in >> b(i);
    }

    return in;
}

// Board b;
// cout << b ;
std::ostream& operator<<(std::ostream& out, Board& b) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            out << std::setw(4) << b(i * N + j);
        }
        out << '\n';
    }
    return out;
}