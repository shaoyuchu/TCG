#include <queue>
#include <set>
#include <string>
#include <vector>

#include "board.h"
using namespace std;

class BruteForceSolver {
   private:
    queue<Board> boardQueue;
    set<string> visited;

    bool isVisited(Board board);

   public:
    BruteForceSolver(Board initialBoard);
    vector<pair<short, Action>> solve();
};
