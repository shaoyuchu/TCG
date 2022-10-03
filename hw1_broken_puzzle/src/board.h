#pragma once
#include <array>
#include <iostream>
#define M 4
#define N 5
using namespace std;

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
   private:
    array<array<int, N>, M> puzzle = {0};

   public:
    enum Action { up, down, left, right, null };

    Board() {}
    int& operator()(int i) { return puzzle[i / N][i % N]; }
    array<int, N>& operator[](int i) { return puzzle[i]; }
    void slide(int i, Action act) { slide(i / N, i % N, act); }
    void slide(int i, int j, Action act);
    bool isCompleted();
};
