#include "a_star_solver.h"

AStarSolver::AStarSolver(Board& initialBoard) {
    this->priorityQueue.push(initialBoard);
    this->visited.insert(initialBoard.toBitset());
}

bool AStarSolver::isClosed(Board& board) {
    return (this->closedList.find(board.toBitset()) != this->closedList.end());
}

vector<pair<short, Action>> AStarSolver::solve() {
    while (!this->priorityQueue.empty()) {
        Board curBoard = this->priorityQueue.top();
        this->priorityQueue.pop();

        // check if the game ends, if not, add curBoard to closed list
        if (curBoard.isCompleted()) return curBoard.getPrevMoves();
        this->closedList.insert(curBoard.toBitset());

        // search deeper
        for (Board nextBoard : curBoard.getNext()) {
            if (nextBoard.isNull()) break;
            if (!this->isVisited(nextBoard)) {
                this->priorityQueue.push(nextBoard);
            } else if (!this->isClosed(nextBoard)) {
                this->priorityQueue.replaceIfSmallerCost(nextBoard);
            }
        }
    }
    cerr << "AStarSolver: Failed to find a solution." << endl;
    return vector<pair<short, Action>>();
}