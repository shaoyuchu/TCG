#pragma once
#include "board.h"

class AStarBoard : public Board {
   protected:
    int estRemaining = 0;

   public:
    void init();
    int getCost() { return this->prevMoves.size() + this->estRemaining; }
    array<Board*, 8> getNext();
    void move(int i, int j, Action act);
    void printDebugMsg() {
        cout << "#previous_step: " << this->prevMoves.size() << endl;
        cout << "#estimated_remaining_step: " << this->estRemaining << endl;
        Board::printDebugMsg();
    }
};