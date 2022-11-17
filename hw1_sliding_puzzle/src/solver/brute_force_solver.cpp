#include "brute_force_solver.h"

void BruteForceSolver::init(Board* initialBoard) {
    this->boardQueue.push(initialBoard);
    this->addToVisited(initialBoard);
}

Board BruteForceSolver::solve() {
    while (!this->boardQueue.empty()) {
        Board* curBoard = this->boardQueue.front();
        if (curBoard->isCompleted()) {
            Board terminalBoard(*curBoard);
            return terminalBoard;
        }
        this->boardQueue.pop();

        // search deeper
        for (Board* nextBoard : curBoard->getNext()) {
            if (nextBoard == NULL) break;
            if (!this->isVisited(nextBoard)) {
                this->boardQueue.push(nextBoard);
                this->addToVisited(nextBoard);
            } else {
                delete nextBoard;
            }
        }
        delete curBoard;
    }

    cerr << "BruteForceSolver: Failed to find a solution." << endl;
    return Board();
}

void BruteForceSolver::deleteAll() {
    while (!this->boardQueue.empty()) {
        delete this->boardQueue.front();
        this->boardQueue.pop();
    }
}