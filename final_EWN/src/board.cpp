#include "board.hpp"
using namespace std;

map<tuple<short, Direction>, Ply> Ply::initPlyInstances() {
    map<tuple<short, Direction>, Ply> plyInstances;
    Direction directions[] = {Direction::Vertical, Direction::Horizontal,
                              Direction::Diagonal};
    for (short cubeId = 0; cubeId < 12; cubeId++) {
        for (Direction& dir : directions) {
            plyInstances.insert(make_pair(make_tuple(cubeId, dir), Ply(cubeId, dir)));
        }
    }
    return plyInstances;
}

const map<tuple<short, Direction>, Ply> Ply::plyInstances = Ply::initPlyInstances();

const Ply& Ply::getPly(short cubeId, Direction dir) {
    return Ply::plyInstances.at(make_tuple(cubeId, dir));
}

const string Board::cubeNames[N_CUBE] = {"B1", "B2", "B3", "B4", "B5", "B6",
                                         "R1", "R2", "R3", "R4", "R5", "R6"};

// get the cube currently occupying cell `cellId`,
// 0~5 is the blue cube 1-6, 6~11 is the red cube 1-6,
// if there is no cube occupying the cell, return -1
int Board::getCubeByCellId(int cellId) const {
    Bitboard mask;
    mask.set(cellId);
    for (int i = 0; i < N_CUBE; i++) {
        if ((this->bitboards[i] & mask).any()) return i;
    }
    return -1;
}

void Board::generateMove(vector<Ply>& result, int dice) {
    // TODO: generate move
}

string Board::toString() const {
    string result;
    int index = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            int cubeId = this->getCubeByCellId(index);
            result += (cubeId == -1 ? "__" : Board::cubeNames[cubeId]);
            index++;
            result += " ";
        }
        result += "\n";
    }
    return result;
}

int Board::getMovableCube(int dice) const {
    // TODO
    return 0;
}

void Board::setCube(int cubeId, int position) { this->bitboards[cubeId].set(position); }
