#include "brute_force_solver.h"

bool BruteForceSolver::isVisited(Board board) {
    return (this->visited.find(board.toString()) != this->visited.end());
}

BruteForceSolver::BruteForceSolver(Board& initialBoard) {
    this->boardQueue.push(initialBoard);
    this->visited.insert(initialBoard.toString());
}

vector<pair<short, Action>> BruteForceSolver::solve() {
    while (!this->boardQueue.empty()) {
        Board curBoard = boardQueue.front();
        boardQueue.pop();

        // check if the game ends
        if (curBoard.isCompleted()) return curBoard.getPrevMoves();

        // search deeper
        for (auto nextBoard : curBoard.getNext()) {
            if (!nextBoard.isNull() and !this->isVisited(nextBoard)) {
                this->boardQueue.push(nextBoard);
            }
        }
    }

    cerr << "BruteForceSolver: Failed to find a solution." << endl;
    return vector<pair<short, Action>>();
}