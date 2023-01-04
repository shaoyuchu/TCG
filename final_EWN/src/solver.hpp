#pragma once
#include <unordered_map>

#include "board.hpp"
#define ATTACK_FACTOR ((double)2.0)
#define THREAT_FACTOR ((double)0.05)
#define MAX_EVAL ((double)396.0)
#define MIN_EVAL ((double)-396.0)
#define MIN_DEPTH 2
#define OPEN_MAX_DEPTH 4
#define MIDDLE_MAX_DEPTH 6
#define END_MAX_DEPTH 6
#define OPEN_PLY_COUNT 2
#define MIDDLE_PLY_COUNT 4
#define IDAS_THRES ((double)5.0)

class Solver {
   private:
    Board baseBoard;
    Color nextTurn;
    static int getCount;
    static unordered_map<size_t, tuple<int, double, double, double>> transpositionTable;
    const static unordered_map<bitset<12>, array<int, 6>> cubeCoverage;
    const static array<int, N_CELL> dist2TargetCorner;
    static unordered_map<bitset<12>, array<int, 6>> initCubeCoverages();
    static void initCubeCoverages(bitset<12> current, int next,
                                  unordered_map<bitset<12>, array<int, 6>>& result);
    static array<int, 6> computeCubeCoverage(bitset<12>& current);
    static tuple<int, double, double, double> lookupTp(Board& board,
                                                       int dice) throw(out_of_range);
    static void insertTp(Board& board, int dice, int depth, double alpha, double beta,
                         double value);
    double star0Max(Board& board, double alpha, double beta, int depth);
    double star0Min(Board& board, double alpha, double beta, int depth);
    double star05Max(Board& board, double alpha, double beta, int depth);
    double star05Min(Board& board, double alpha, double beta, int depth);
    double star1Max(Board& board, double alpha, double beta, int depth);
    double star1Min(Board& board, double alpha, double beta, int depth);
    double negaScoutMax(Board& board, int dice, double alpha, double beta, int depth);
    double negaScoutMin(Board& board, int dice, double alpha, double beta, int depth);

   public:
    Solver(Board& board) : baseBoard(board), nextTurn(board.getNextTurn()) {}
    Ply getBestPly(int dice);
    double evaluateBoard(const Board& board);
    static void clearTp() { Solver::transpositionTable.clear(); }
    static void clearGetCount() { Solver::getCount = 0; }
};