#include "a_star_solver.h"

void PQWithRemove::replaceIfSmallerCost(Board& replaceBy) {
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

AStarSolver::AStarSolver(Board& initialBoard) {
    this->priorityQueue.push(initialBoard);
    this->addToVisited(initialBoard);
}

vector<pair<short, Action>> AStarSolver::solve() {
    while (!this->priorityQueue.empty()) {
        Board curBoard = this->priorityQueue.top();
        this->priorityQueue.pop();

        // check if the game ends, if not, add curBoard to closed list
        if (curBoard.isCompleted()) return curBoard.getPrevMoves();

        // search deeper
        for (Board nextBoard : curBoard.getNext()) {
            if (nextBoard.isNull()) break;
            if (!this->isVisited(nextBoard)) {
                this->priorityQueue.push(nextBoard);
                this->addToVisited(nextBoard);
            } else {
                this->priorityQueue.replaceIfSmallerCost(nextBoard);
            }
        }
    }
    cerr << "AStarSolver: Failed to find a solution." << endl;
    return vector<pair<short, Action>>();
}