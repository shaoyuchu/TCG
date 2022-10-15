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
    void replaceIfSmallerCost(Board& replaceBy);
};

class AStarSolver : public PuzzleSolver {
   private:
    PQWithRemove priorityQueue;

   public:
    AStarSolver(Board& initialBoard);
    vector<pair<short, Action>> solve();
};