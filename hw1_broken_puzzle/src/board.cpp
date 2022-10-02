#include "board.h"
using namespace std;

void Board::slide(int i, int j, Action act) {
    if (puzzle[i][j] == 0 || puzzle[i][j] == -1) {
        // moving an empty cell or a non-movable tile
        cout << "Invalid slide" << '\n';
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

bool Board::isCompleted() {
    for (int i = 0; i < M * N; i++) {
        if (!(this->operator()(i) == i + 1 || this->operator()(i) == 0 ||
              this->operator()(i) == -1))
            return false;
    }
    return true;
}
