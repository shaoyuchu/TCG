#include <vector>

#include "board.h"

class PuzzleSolver {
   protected:
    unordered_set<bitset<BITSET_LEN>> visited;
    bool isVisited(Board board) {
        return (this->visited.find(board.toBitset()) != this->visited.end());
    }

   public:
    virtual ~PuzzleSolver() = default;
    virtual vector<pair<short, Action>> solve() = 0;
};