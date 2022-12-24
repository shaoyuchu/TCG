#pragma once
#include "board.hpp"

class Solver {
   private:
    Board board;

   public:
    Solver(Board& board) : board(board) {}
    Ply getBestPly(int dice);
};