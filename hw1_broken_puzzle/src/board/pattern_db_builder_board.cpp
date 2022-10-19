#include "pattern_db_builder_board.h"

bool PatternDbBuilderBoard::isCompleted() {
    for (int i = 0; i < M * N - N_EMPTY; i++) {
        if (!this->targetPattern[i + 1]) continue;
        if (!(this->get(i) == i + 1 || this->get(i) < 0)) return false;
    }
    return true;
}

void PatternDbBuilderBoard::init(vector<int>& pattern) {
    this->estRemaining = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int cellValue = this->puzzle[i][j];
            if (cellValue <= 0 || !this->targetPattern[cellValue - 1]) continue;
            int targetPos = cellValue - 1;
            this->estRemaining += abs(i - (targetPos / N)) + abs(j - (targetPos % N));
        }
    }
    for (int& num : pattern) {
        if (num < 0 || num >= M * N) {
            cerr << "Error: Invalid pattern." << endl;
            exit(1);
        }
        this->targetPattern[num] = true;
    }
}

void PatternDbBuilderBoard::move(int i, int j, Action action) {
    int movingNum = puzzle[i][j];
    int targetPos = movingNum - 1;
    Board::slide(i, j, action);
    if (movingNum > 0 && movingNum < M * N && this->targetPattern[movingNum]) {
        Board::appendMove(i, j, action);
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

Board* PatternDbBuilderBoard::duplicate() {
    Board* newBoard = new PatternDbBuilderBoard(*this);
    this->clearCache();
    return newBoard;
}