#pragma once
#include "board.hpp"

class Solver {
   private:
    Board board;
    const static map<array<bool, 6>, array<int, 6>> cubeCoverage;
    static map<array<bool, 6>, array<int, 6>> initCubeCoverages();
    static void initCubeCoverages(array<bool, 6> current, int next,
                                  map<array<bool, 6>, array<int, 6>>& result);
    static array<int, 6> computeCubeCoverage(array<bool, 6>& current);
    int evalAbsPos(const Board& board);
    int evalThreat(const Board& board);

   public:
    Solver(Board& board) : board(board) {}
    int evaluateBoard(const Board& board);
    Ply getBestPly(int dice);
};