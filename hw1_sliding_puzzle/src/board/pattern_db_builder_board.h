#pragma once
#include "a_star_board.h"

class PatternDbBuilderBoard : public AStarBoard {
   private:
    array<bool, M* N> targetPattern = {0};

    Board* duplicate();

   public:
    void setPattern(vector<int>& pattern);
    void init();                             // override the estimation method
    void move(int i, int j, Action action);  // override the estimation update method
    bool isCompleted();
};