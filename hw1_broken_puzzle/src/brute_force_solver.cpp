#include "brute_force_solver.h"

BruteForceSolver::BruteForceSolver(Board& initialBoard) {
    this->boardQueue.push(initialBoard);
    this->addToVisited(initialBoard);
}

vector<pair<short, Action>> BruteForceSolver::solve() {
    while (!this->boardQueue.empty()) {
        Board curBoard = this->boardQueue.front();
        this->boardQueue.pop();

        // check if the game ends
        if (curBoard.isCompleted()) return curBoard.getPrevMoves();

        // search deeper
        for (auto nextBoard : curBoard.getNext()) {
            if (nextBoard.isNull()) break;
            if (!this->isVisited(nextBoard)) {
                this->boardQueue.push(nextBoard);
                this->addToVisited(nextBoard);
            }
        }
    }

    cerr << "BruteForceSolver: Failed to find a solution." << endl;
    return vector<pair<short, Action>>();
}