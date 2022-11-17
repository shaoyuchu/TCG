#pragma once
#include "board.h"

class AStarBoard : public Board {
   private:
    Board* duplicate();

   protected:
    int estRemaining = 0;

   public:
    void init();
    int getCost() { return this->prevMoves.size() + this->estRemaining; }
    void move(int i, int j, Action act);
    void printDebugMsg() {
        cout << "#previous_step: " << this->prevMoves.size() << endl;
        cout << "#estimated_remaining_step: " << this->estRemaining << endl;
        Board::printDebugMsg();
    }
};