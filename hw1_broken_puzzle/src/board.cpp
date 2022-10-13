#include "board.h"
using namespace std;

/*Private Method*/

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
    this->prevMoves.push_back(make_pair(convert2dTo1d(i, j), act));
}

/*Public Method*/

vector<Board> Board::getNext() {
    vector<Board> next;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (this->puzzle[i][j] != 0) continue;
            // (i-1, j) move down
            if (i - 1 >= 0 && this->puzzle[i - 1][j] > 0) {
                Board movedDown(*this);
                movedDown.move(i - 1, j, Action::down);
                next.push_back(movedDown);
            }
            // (i+1, j) move up
            if (i + 1 < M && this->puzzle[i + 1][j] > 0) {
                Board movedUp(*this);
                movedUp.move(i + 1, j, Action::up);
                next.push_back(movedUp);
            }
            // (i, j-1) move right
            if (j - 1 >= 0 && this->puzzle[i][j - 1] > 0) {
                Board movedRight(*this);
                movedRight.move(i, j - 1, Action::right);
                next.push_back(movedRight);
            }
            // (i, j+1) move left
            if (j + 1 < N && this->puzzle[i][j + 1] > 0) {
                Board movedLeft(*this);
                movedLeft.move(i, j + 1, Action::left);
                next.push_back(movedLeft);
            }
        }
    }
    return next;
}

// Apply `action` to cell (i, j) and add the move to `prevMoves`
void Board::move(int i, int j, Action action) {
    this->slide(i, j, action);
    this->appendMove(i, j, action);
}

bool Board::isCompleted() {
    for (int i = 0; i < M * N; i++) {
        if (!(this->operator()(i) == i + 1 || this->operator()(i) == 0 ||
              this->operator()(i) == -1))
            return false;
    }
    return true;
}

string Board::toString() {
    string result = "";
    for (int i = 0; i < M * N; i++) {
        result += to_string(this->operator()(i)) + "/";
    }
    return result;
}