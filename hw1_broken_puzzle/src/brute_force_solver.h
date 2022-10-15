#include <queue>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "board.h"
using namespace std;

class BruteForceSolver {
   private:
    queue<Board> boardQueue;
    unordered_set<bitset<BITSET_LEN>> visited;

    bool isVisited(Board board);

   public:
    BruteForceSolver(Board& initialBoard);
    vector<pair<short, Action>> solve();
};
