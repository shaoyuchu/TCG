#include "a_star_solver.h"

void PQWithRemove::replaceIfSmallerCost(Board* replaceBy) {
    for (int i = this->c.size() - 1; i >= 0; i--) {
        // no existance with higher cost
        if (this->c[i]->getEstTotalCost() <= replaceBy->getEstTotalCost()) return;

        // remove if the same puzzle exists
        if (this->c[i]->toBitset() == replaceBy->toBitset()) {
            delete this->c[i];
            this->c.erase(this->c.begin() + i);
            this->push(replaceBy);
            return;
        }
    }
}

void PQWithRemove::deleteAll() {
    for (int i = 0; i < this->c.size(); i++) {
        delete this->c[i];
    }
}

AStarSolver::AStarSolver(Board* initialBoard) {
    initialBoard->initEstRemaining();
    this->priorityQueue.push(initialBoard);
    this->addToVisited(initialBoard);
}

Board* AStarSolver::solve() {
    while (!this->priorityQueue.empty()) {
        Board* curBoard = this->priorityQueue.top();
        if (curBoard->isCompleted()) return curBoard;
        this->priorityQueue.pop();

        // search deeper
        for (Board* nextBoard : curBoard->getNext(true)) {
            if (nextBoard == NULL) break;
            if (!this->isVisited(nextBoard)) {
                this->priorityQueue.push(nextBoard);
                this->addToVisited(nextBoard);
            } else {
                this->priorityQueue.replaceIfSmallerCost(nextBoard);
            }
        }
        delete curBoard;
    }
    cerr << "AStarSolver: Failed to find a solution." << endl;
    return NULL;
}

void AStarSolver::deleteAll() { this->priorityQueue.deleteAll(); }