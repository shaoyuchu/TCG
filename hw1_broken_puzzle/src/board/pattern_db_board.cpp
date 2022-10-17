#include "pattern_db_board.h"

bool PatternDbBoard::isCompleted() {
    for (int i = 0; i < M * N - N_EMPTY; i++) {
        if (!this->targetPattern[i]) continue;
        if (!(this->operator()(i) == i + 1 || this->operator()(i) <= 0)) return false;
    }
    return true;
}

void PatternDbBoard::init() {
    this->estRemaining = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int cellValue = this->puzzle[i][j];
            if (cellValue <= 0 || !this->targetPattern[cellValue - 1]) continue;
            int targetPos = cellValue - 1;
            this->estRemaining += abs(i - (targetPos / N)) + abs(j - (targetPos % N));
        }
    }
}

void PatternDbBoard::move(int i, int j, Action action) {
    int targetPos = puzzle[i][j] - 1;
    Board::move(i, j, action);
    if (this->targetPattern[targetPos]) {
        if (action == Action::up)
            this->estRemaining += (i > (targetPos / N) ? -1 : 1);
        else if (action == Action::down)
            this->estRemaining += (i < (targetPos / N) ? -1 : 1);
        else if (action == Action::left)
            this->estRemaining += (j > (targetPos % N) ? -1 : 1);
        else if (action == Action::right)
            this->estRemaining += (j < (targetPos % N) ? -1 : 1);
    }
}