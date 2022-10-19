#include "a_star_solver.h"

#include "../board/a_star_board.h"

// check if any board with the same puzzle exists in the priority queue
bool PQWithRemove::exist(Board* board) {
    return (this->items.find(board->toBitset()) != this->items.end());
}

void PQWithRemove::replaceIfSmallerCost(Board* replaceBy) {
    for (int i = this->c.size() - 1; i >= 0; i--) {
        // no existance with higher cost
        if (this->c[i]->getCost() <= replaceBy->getCost()) return;

        // remove if the same puzzle exists
        if (this->c[i]->toBitset() == replaceBy->toBitset()) {
            delete this->c[i];
            this->c.erase(this->c.begin() + i);
            this->push(replaceBy);
            return;
        }
    }
}

void PQWithRemove::push(Board* board) {
    priority_queue<Board*, vector<Board*>, CostComparison>::push(board);
    this->items.insert(board->toBitset());
}

void PQWithRemove::pop() {
    Board* top = this->top();
    priority_queue<Board*, vector<Board*>, CostComparison>::pop();
    this->items.erase(top->toBitset());
}

void PQWithRemove::deleteAll() {
    for (int i = 0; i < this->c.size(); i++) {
        delete this->c[i];
    }
}

void AStarSolver::init(Board* initialBoard) {
    initialBoard->init();
    this->priorityQueue.push(initialBoard);
    this->addToVisited(initialBoard);
}

Board AStarSolver::solve() {
    while (!this->priorityQueue.empty()) {
        Board* curBoard = this->priorityQueue.top();
        if (curBoard->isCompleted()) {
            Board terminalBoard(*curBoard);
            return terminalBoard;
        }
        this->priorityQueue.pop();

        // search deeper
        for (Board* nextBoard : curBoard->getNext()) {
            if (nextBoard == NULL) break;
            if (!this->isVisited(nextBoard)) {
                this->priorityQueue.push(nextBoard);
                this->addToVisited(nextBoard);
            } else if (this->priorityQueue.exist(nextBoard)) {
                this->priorityQueue.replaceIfSmallerCost(nextBoard);
            }
        }
        delete curBoard;
    }
    cerr << "AStarSolver: Failed to find a solution." << endl;
    return Board();
}

void AStarSolver::deleteAll() { this->priorityQueue.deleteAll(); }