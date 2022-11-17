#pragma once
#include <bitset>
#include <unordered_map>

#include "a_star_board.h"

class PatternDatabaseBoard : public AStarBoard {
   private:
    unordered_map<bitset<M * N>, unordered_map<bitset<BITSET_LEN>, int>>* patterns = NULL;

    int getPatternHeuristicCost();
    Board* duplicate();

   public:
    void setPatterns(
        unordered_map<bitset<M * N>, unordered_map<bitset<BITSET_LEN>, int>>* patterns);
    int getCost();
};