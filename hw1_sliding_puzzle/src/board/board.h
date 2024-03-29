#pragma once
#include <array>
#include <bitset>
#include <iostream>
#include <utility>
#include <vector>
#define M 4
#define N 5
#define N_EMPTY 2
#define BIT_PER_CELL 5
#define BITSET_LEN (BIT_PER_CELL * M * N)
#define CONVERT_2D_2_1D(i, j) (i * N + j)
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

enum Action { up, down, left, right, null };

class Board {
   private:
    int& operator()(int i) { return puzzle[i / N][i % N]; }

   protected:
    array<array<int, N>, M> puzzle = {0};
    vector<pair<short, Action>> prevMoves;
    static bitset<BITSET_LEN> cellBitMask;
    bitset<BITSET_LEN> bitsetCache;

    void slide(int i, int j, Action act);
    virtual void appendMove(int i, int j, Action act);
    virtual Board* duplicate();

   public:
    Board() { this->bitsetCache = bitset<BITSET_LEN>(0); }
    int get(int i) { return this->operator()(i); }
    void set(int i, int value);
    void clearCache() { this->bitsetCache = bitset<BITSET_LEN>(0); }
    vector<pair<short, Action>>& getPrevMoves() { return this->prevMoves; }
    bitset<BITSET_LEN> toBitset();
    virtual bool isCompleted();
    virtual void init() {}
    virtual int getCost() { return -1; };
    virtual array<Board*, 8> getNext();
    virtual void move(int i, int j, Action act);
    virtual void printDebugMsg();
};
