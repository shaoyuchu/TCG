#pragma once
#include "a_star_board.h"

class PatternDbBoard : public AStarBoard {
   private:
    array<bool, M* N> targetPattern = {0};

   public:
    void init(vector<int>& pattern);         // override the estimation method
    void move(int i, int j, Action action);  // override the estimation update method
    array<Board*, 8> getNext();
    bool isCompleted();
};