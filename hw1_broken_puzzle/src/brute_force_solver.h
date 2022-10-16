#include <queue>

#include "board.h"
#include "puzzle_solver.h"
using namespace std;

class BruteForceSolver : public PuzzleSolver {
   private:
    queue<Board*> boardQueue;

   public:
    BruteForceSolver(Board* initialBoard);
    Board* solve();
    void deleteAll();
};
