#pragma once
#include <unordered_map>

#include "board.hpp"
#define ATTACK_FACTOR 2

class Solver {
   private:
    const static unordered_map<bitset<12>, array<int, 6>> cubeCoverage;
    const static array<int, N_CELL> dist2TargetCorner;
    static unordered_map<bitset<12>, array<int, 6>> initCubeCoverages();
    static void initCubeCoverages(bitset<12> current, int next,
                                  unordered_map<bitset<12>, array<int, 6>>& result);
    static array<int, 6> computeCubeCoverage(bitset<12>& current);
    int evalAbsPos(const Board& board);
    int evalThreat(const Board& board);

   public:
    int evaluateBoard(const Board& board);
    Ply getBestPly(Board& board, int dice);
};