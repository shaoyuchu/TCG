#include "pattern_db_board.h"

bool PatternDbBoard::isCompleted() {
    for (int i = 0; i < M * N - N_EMPTY; i++) {
        if (!this->targetPattern[i + 1]) continue;
        if (!(this->operator()(i) == i + 1 || this->operator()(i) < 0)) return false;
    }
    return true;
}

void PatternDbBoard::init(vector<int>& pattern) {
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

void PatternDbBoard::move(int i, int j, Action action) {
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

array<Board*, 8> PatternDbBoard::getNext() {
    array<Board*, 8> next = {NULL};
    int curCnt = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (this->puzzle[i][j] != 0) continue;
            // (i-1, j) move down
            if (i - 1 >= 0 && this->puzzle[i - 1][j] > 0) {
                Board* movedDown = new PatternDbBoard(*this);
                movedDown->move(i - 1, j, Action::down);
                next[curCnt] = movedDown;
                curCnt++;
            }
            // (i+1, j) move up
            if (i + 1 < M && this->puzzle[i + 1][j] > 0) {
                Board* movedUp = new PatternDbBoard(*this);
                movedUp->move(i + 1, j, Action::up);
                next[curCnt] = movedUp;
                curCnt++;
            }
            // (i, j-1) move right
            if (j - 1 >= 0 && this->puzzle[i][j - 1] > 0) {
                Board* movedRight = new PatternDbBoard(*this);
                movedRight->move(i, j - 1, Action::right);
                next[curCnt] = movedRight;
                curCnt++;
            }
            // (i, j+1) move left
            if (j + 1 < N && this->puzzle[i][j + 1] > 0) {
                Board* movedLeft = new PatternDbBoard(*this);
                movedLeft->move(i, j + 1, Action::left);
                next[curCnt] = movedLeft;
                curCnt++;
            }
        }
    }
    return next;
}