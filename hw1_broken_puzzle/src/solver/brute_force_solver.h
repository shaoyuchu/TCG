#include <queue>

#include "../board/board.h"
#include "puzzle_solver.h"
using namespace std;

class BruteForceSolver : public PuzzleSolver {
   private:
    queue<Board*> boardQueue;

   public:
    void init(Board* initialBoard);
    Board* solve();
    void deleteAll();
};
