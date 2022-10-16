#include "brute_force_solver.h"

void BruteForceSolver::init(Board* initialBoard) {
    this->boardQueue.push(initialBoard);
    this->addToVisited(initialBoard);
}

Board* BruteForceSolver::solve() {
    while (!this->boardQueue.empty()) {
        Board* curBoard = this->boardQueue.front();
        if (curBoard->isCompleted()) return curBoard;
        this->boardQueue.pop();

        // search deeper
        for (Board* nextBoard : curBoard->getNext()) {
            if (nextBoard == NULL) break;
            if (!this->isVisited(nextBoard)) {
                this->boardQueue.push(nextBoard);
                this->addToVisited(nextBoard);
            }
        }
        delete curBoard;
    }

    cerr << "BruteForceSolver: Failed to find a solution." << endl;
    return NULL;
}

void BruteForceSolver::deleteAll() {
    while (!this->boardQueue.empty()) {
        delete this->boardQueue.front();
        this->boardQueue.pop();
    }
}