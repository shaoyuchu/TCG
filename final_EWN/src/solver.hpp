#pragma once
#include <unordered_map>

#include "board.hpp"
#define ATTACK_FACTOR 2.0
#define THREAT_FACTOR 0.05
#define MAX_DEPTH 3

class Solver {
   private:
    Board baseBoard;
    Color nextTurn;
    const static unordered_map<bitset<12>, array<int, 6>> cubeCoverage;
    const static array<int, N_CELL> dist2TargetCorner;
    static unordered_map<bitset<12>, array<int, 6>> initCubeCoverages();
    static void initCubeCoverages(bitset<12> current, int next,
                                  unordered_map<bitset<12>, array<int, 6>>& result);
    static array<int, 6> computeCubeCoverage(bitset<12>& current);
    double evaluateBoard(const Board& board);
    double star0(Board board, double alpha, double beta, int currentDepth,
                 int remainingDepth);
    double negaScout(Board board, int dice, double alpha, double beta, int currentDepth,
                     int remainingDepth);

   public:
    Solver(Board& board) : baseBoard(board), nextTurn(board.getNextTurn()) {}
    Ply getBestPly(int dice);
};