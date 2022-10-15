#include <queue>
#include <unordered_set>
#include <vector>

#include "board.h"
#include "puzzle_solver.h"
using namespace std;

class BruteForceSolver : public PuzzleSolver {
   private:
    queue<Board> boardQueue;
    unordered_set<bitset<BITSET_LEN>> visited;

    bool isVisited(Board board);

   public:
    BruteForceSolver(Board& initialBoard);
    vector<pair<short, Action>> solve();
};
