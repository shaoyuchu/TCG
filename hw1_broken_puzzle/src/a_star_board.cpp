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

array<Board*, 8> AStarBoard::getNext() {
    array<Board*, 8> next = {NULL};
    int curCnt = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (this->puzzle[i][j] != 0) continue;
            // (i-1, j) move down
            if (i - 1 >= 0 && this->puzzle[i - 1][j] > 0) {
                Board* movedDown = new AStarBoard(*this);
                movedDown->move(i - 1, j, Action::down);
                next[curCnt] = movedDown;
                curCnt++;
            }
            // (i+1, j) move up
            if (i + 1 < M && this->puzzle[i + 1][j] > 0) {
                Board* movedUp = new AStarBoard(*this);
                movedUp->move(i + 1, j, Action::up);
                next[curCnt] = movedUp;
                curCnt++;
            }
            // (i, j-1) move right
            if (j - 1 >= 0 && this->puzzle[i][j - 1] > 0) {
                Board* movedRight = new AStarBoard(*this);
                movedRight->move(i, j - 1, Action::right);
                next[curCnt] = movedRight;
                curCnt++;
            }
            // (i, j+1) move left
            if (j + 1 < N && this->puzzle[i][j + 1] > 0) {
                Board* movedLeft = new AStarBoard(*this);
                movedLeft->move(i, j + 1, Action::left);
                next[curCnt] = movedLeft;
                curCnt++;
            }
        }
    }
    return next;
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