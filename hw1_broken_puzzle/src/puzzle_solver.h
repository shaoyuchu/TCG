#pragma once
#include <unordered_set>

#include "board/board.h"

class PuzzleSolver {
   private:
    unordered_set<bitset<BITSET_LEN>> visited;

   protected:
    void addToVisited(Board* board) { this->visited.insert(board->toBitset()); }
    bool isVisited(Board* board) {
        return (this->visited.find(board->toBitset()) != this->visited.end());
    }

   public:
    virtual ~PuzzleSolver() = default;
    virtual void init(Board* initialBoard) = 0;
    virtual Board* solve() = 0;
    virtual void deleteAll() = 0;
};