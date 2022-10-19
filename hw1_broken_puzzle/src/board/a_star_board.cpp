#include "a_star_board.h"

void AStarBoard::init() {
    this->estRemaining = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int cellValue = this->puzzle[i][j];
            if (cellValue <= 0) continue;
            int targetPos = cellValue - 1;
            this->estRemaining += abs(i - (targetPos / N)) + abs(j - (targetPos % N));
        }
    }
}

Board* AStarBoard::duplicate() {
    Board* newBoard = new AStarBoard(*this);
    return newBoard;
}

void AStarBoard::move(int i, int j, Action action) {
    int targetPos = puzzle[i][j] - 1;
    Board::move(i, j, action);
    if (action == Action::up)
        this->estRemaining += (i > (targetPos / N) ? -1 : 1);
    else if (action == Action::down)
        this->estRemaining += (i < (targetPos / N) ? -1 : 1);
    else if (action == Action::left)
        this->estRemaining += (j > (targetPos % N) ? -1 : 1);
    else if (action == Action::right)
        this->estRemaining += (j < (targetPos % N) ? -1 : 1);
}