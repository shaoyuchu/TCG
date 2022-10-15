#include <vector>

#include "board.h"

class PuzzleSolver {
   public:
    virtual ~PuzzleSolver() = default;
    virtual vector<pair<short, Action>> solve() = 0;
};