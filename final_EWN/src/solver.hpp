#pragma once
#include "board.hpp"

class Solver {
   private:
    Board board;

   public:
    Ply getBestPly(int dice);
};