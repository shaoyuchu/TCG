#pragma once
#include <bitset>
#include <string>
#include <unordered_map>

#include "../board/pattern_database_board.h"
#include "../pattern_database.h"
#include "a_star_solver.h"

class PatternDatabaseSolver : public AStarSolver {
   private:
    unordered_map<bitset<M * N>, unordered_map<bitset<BITSET_LEN>, int>> patterns;

   public:
    PatternDatabaseSolver(string configPath);
    void init(Board* initialBoard);
};