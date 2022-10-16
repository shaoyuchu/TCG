#include "board.h"

#include <stdlib.h>

#include <iomanip>
using namespace std;

/*Private*/

bitset<BITSET_LEN> Board::cellBitMask = bitset<BITSET_LEN>((1 << BIT_PER_CELL) - 1);

void Board::slide(int i, int j, Action act) {
    if (puzzle[i][j] == 0 || puzzle[i][j] == -1) {
        // moving an empty cell or a non-movable tile
        cout << "Invalid slide (moving " << i << "-" << j << ")\n";
        exit(-1);
    }

    if ((act == Action::up) && (i - 1 >= 0) && (puzzle[i - 1][j] == 0))
        swap(puzzle[i][j], puzzle[i - 1][j]);
    else if ((act == Action::down) && (i + 1 < M) && (puzzle[i + 1][j] == 0))
        swap(puzzle[i][j], puzzle[i + 1][j]);
    else if ((act == Action::left) && (j - 1 >= 0) && (puzzle[i][j - 1] == 0))
        swap(puzzle[i][j], puzzle[i][j - 1]);
    else if ((act == Action::right) && (j + 1 < N) && (puzzle[i][j + 1] == 0))
        swap(puzzle[i][j], puzzle[i][j + 1]);
    else {
        cout << act << '\n';
        cout << "Invalid slide\n";
        exit(-1);
    }
}

void Board::appendMove(int i, int j, Action act) {
    this->prevMoves.push_back(make_pair(CONVERT_2D_2_1D(i, j), act));
}

// Apply `action` to cell (i, j), add the move to `prevMoves`, and update `estRemaining`
void Board::move(int i, int j, Action action) {
    int targetPos = puzzle[i][j] - 1;
    this->slide(i, j, action);
    this->appendMove(i, j, action);
    if (action == Action::up)
        this->estRemaining += (i > (targetPos / N) ? -1 : 1);
    else if (action == Action::down)
        this->estRemaining += (i < (targetPos / N) ? -1 : 1);
    else if (action == Action::left)
        this->estRemaining += (j > (targetPos % N) ? -1 : 1);
    else if (action == Action::right)
        this->estRemaining += (j < (targetPos % N) ? -1 : 1);
}

/*Public*/

void Board::initEstRemaining() {
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

array<Board, 8> Board::getNext() {
    array<Board, 8> next;
    int curCnt = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (this->puzzle[i][j] != 0) continue;
            // (i-1, j) move down
            if (i - 1 >= 0 && this->puzzle[i - 1][j] > 0) {
                Board movedDown(*this);
                movedDown.move(i - 1, j, Action::down);
                next[curCnt] = movedDown;
                curCnt++;
            }
            // (i+1, j) move up
            if (i + 1 < M && this->puzzle[i + 1][j] > 0) {
                Board movedUp(*this);
                movedUp.move(i + 1, j, Action::up);
                next[curCnt] = movedUp;
                curCnt++;
            }
            // (i, j-1) move right
            if (j - 1 >= 0 && this->puzzle[i][j - 1] > 0) {
                Board movedRight(*this);
                movedRight.move(i, j - 1, Action::right);
                next[curCnt] = movedRight;
                curCnt++;
            }
            // (i, j+1) move left
            if (j + 1 < N && this->puzzle[i][j + 1] > 0) {
                Board movedLeft(*this);
                movedLeft.move(i, j + 1, Action::left);
                next[curCnt] = movedLeft;
                curCnt++;
            }
        }
    }
    return next;
}

bool Board::isCompleted() {
    for (int i = 0; i < M * N; i++) {
        if (!(this->operator()(i) == i + 1 || this->operator()(i) <= 0)) return false;
    }
    return true;
}

bool Board::isNull() {
    for (int j = 0; j < N; j++)
        if (puzzle[0][j] > 0) return false;
    return true;
}

bitset<BITSET_LEN> Board::toBitset() {
    bitset<BITSET_LEN> result;
    for (int i = 0; i < M * N; i++) {
        bitset<BITSET_LEN> current(this->operator()(i));
        current &= this->cellBitMask;
        result |= (current << (i * BIT_PER_CELL));
    }
    return result;
}

void Board::printDebugMsg() {
    cout << "#previous_step: " << this->prevMoves.size() << endl;
    cout << "#estimated_remaining_step: " << this->estRemaining << endl;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            cout << setw(4) << puzzle[i][j];
        }
        cout << '\n';
    }
    cout << endl;
}