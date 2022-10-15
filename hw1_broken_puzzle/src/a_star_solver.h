#include <queue>
#include <unordered_set>
#include <vector>

#include "board.h"
#include "puzzle_solver.h"
using namespace std;

class CostComparison {
   public:
    bool operator()(Board left, Board right) {
        return left.getEstTotalCost() > right.getEstTotalCost();
    }
};

class PQWithRemove : public priority_queue<Board, vector<Board>, CostComparison> {
   public:
    void replaceIfSmallerCost(Board replaceBy) {
        for (int i = c.size() - 1; i >= 0; i++) {
            // no existance with higher cost
            if (this->c[i].getEstTotalCost() <= replaceBy.getEstTotalCost()) return;

            // remove if the same puzzle exists
            if (this->c[i].toBitset() == replaceBy.toBitset()) {
                this->c.erase(this->c.begin() + i);
                this->push(replaceBy);
                return;
            }
        }
    }
};

class AStarSolver : public PuzzleSolver {
   private:
    PQWithRemove priorityQueue;
    unordered_set<bitset<BITSET_LEN>> closedList;

    bool isClosed(Board& board);

   public:
    AStarSolver(Board& initialBoard);
    vector<pair<short, Action>> solve();
};