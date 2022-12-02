#include <climits>
#include <iostream>

#include "board.hpp"
using namespace std;

void readAndSetInitialPosition(Board& board) {
    int redPos[6][2] = {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 0}};
    int bluePos[6][2] = {{3, 6}, {4, 5}, {4, 6}, {5, 4}, {5, 5}, {5, 6}};
    for (int i = 0; i < 6; i++)
        board.setCube(redPos[i][0], redPos[i][1], Cube(Color::Red, getchar()));
    for (int i = 0; i < 6; i++)
        board.setCube(bluePos[i][0], bluePos[i][1], Cube(Color::Blue, getchar()));
}

void readAndSetPlayer(Board& board) {
    // always play as the Red
    char player = getchar();

    if (player == 's') {
        // flip the board
        board.flip();
        board.flipNextTurn();
    }
}

Direction char2Direction(char directionChar) {
    if (directionChar == '0') return Direction::Vertical;
    if (directionChar == '1') return Direction::Horizontal;
    return Direction::Diagonal;
}

void readPlyAndApply(Board& initialBoard) {
    char num = getchar(), dir = getchar();
    cerr << "Apply input ply " << num << " " << dir << endl;
    if (num == '?' && dir == '?') {
        initialBoard.applyPly(Ply::getPly(0, 0, 0, Direction::None));
    } else {
        initialBoard.applyPly(num, char2Direction(dir));
    }
}

Ply& getBestPly(Board board, int simCnt) {
    vector<Ply>* validPlys = board.getAllValidPly();

    int maxScore = INT_MIN;
    Ply& bestPly = validPlys->at(0);
    for (int i = 0; i < validPlys->size(); i++) {
        Board copiedBoard(board);
        copiedBoard.applyPly(validPlys->at(i));
        int score = 0;
        for (int i = 0; i < simCnt; i++) {
            Color winner = copiedBoard.getRandomPlayWinner();
            if (winner == Color::Red)
                score += 1;
            else if (winner == Color::Blue)
                score -= 1;
        }
        if (score >= maxScore) {
            maxScore = score;
            bestPly = validPlys->at(i);
        }
    }
    return bestPly;
}

void writePlyAndApply(Ply& ply, Board& board) {
    if (ply.dir == Direction::None) {
        cout << "??";
    } else if (ply.dir == Direction::Vertical) {
        cout << ply.num << '0';
    } else if (ply.dir == Direction::Horizontal) {
        cout << ply.num << '1';
    } else if (ply.dir == Direction::Diagonal) {
        cout << ply.num << '2';
    }
    flush(cout);

    board.applyPly(ply);
    cerr << "Apply ply " << ply << endl;
}

int main() {
    while (true) {
        Board initialBoard;
        readAndSetInitialPosition(initialBoard);
        readAndSetPlayer(initialBoard);

        while (!initialBoard.isCompleted()) {
            cerr << endl << initialBoard;
            if (initialBoard.getNextTurn() == Color::Red) {
                Ply bestPly = getBestPly(initialBoard, 10000);
                writePlyAndApply(bestPly, initialBoard);
            } else {
                readPlyAndApply(initialBoard);
            }
        }
        if (getchar() != 'y') break;
    }

    return 0;
}