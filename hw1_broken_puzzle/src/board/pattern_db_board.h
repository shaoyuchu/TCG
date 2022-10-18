#pragma once
#include "a_star_board.h"

class PatternDbBoard : public AStarBoard {
   private:
    array<bool, M* N> targetPattern = {0};

   public:
    void init();                             // override the estimation method
    void move(int i, int j, Action action);  // override the estimation update method
    bool isCompleted();
};