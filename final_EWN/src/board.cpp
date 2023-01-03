#include "board.hpp"

#include <cassert>
#include <iostream>
#include <random>
using namespace std;

map<tuple<int, Direction>, Ply> Ply::initPlyInstances() {
    map<tuple<int, Direction>, Ply> plyInstances;
    Direction directions[] = {Direction::Vertical, Direction::Horizontal,
                              Direction::Diagonal};
    for (int cubeId = 0; cubeId < 12; cubeId++) {
        for (Direction& dir : directions) {
            plyInstances.insert(make_pair(make_tuple(cubeId, dir), Ply(cubeId, dir)));
        }
    }
    return plyInstances;
}

const map<tuple<int, Direction>, Ply> Ply::plyInstances = Ply::initPlyInstances();

const Ply& Ply::getPly(int cubeId, Direction dir) {
    return Ply::plyInstances.at(make_tuple(cubeId, dir));
}

string Ply::toString() const {
    string direction =
        (this->dir == Direction::Vertical
             ? "Vertical"
             : (this->dir == Direction::Horizontal ? "Horizontal" : "Diagonal"));
    return Board::cubeNames[this->cubeId] + " " + direction;
}

const string Board::cubeNames[N_CUBE] = {"B1", "B2", "B3", "B4", "B5", "B6",
                                         "R1", "R2", "R3", "R4", "R5", "R6"};

map<tuple<Color, Direction>, Bitboard> Board::initMoveMasks() {
    map<tuple<Color, Direction>, Bitboard> result;
    result.insert(make_pair(make_tuple(Color::Blue, Direction::Vertical),
                            Bitboard(0b1111111111111111111100000)));
    result.insert(make_pair(make_tuple(Color::Blue, Direction::Horizontal),
                            Bitboard(0b1111011110111101111011110)));
    result.insert(make_pair(make_tuple(Color::Blue, Direction::Diagonal),
                            Bitboard(0b1111011110111101111000000)));
    result.insert(make_pair(make_tuple(Color::Red, Direction::Vertical),
                            Bitboard(0b0000011111111111111111111)));
    result.insert(make_pair(make_tuple(Color::Red, Direction::Horizontal),
                            Bitboard(0b0111101111011110111101111)));
    result.insert(make_pair(make_tuple(Color::Red, Direction::Diagonal),
                            Bitboard(0b0000001111011110111101111)));
    return result;
}

unordered_map<Bitboard, Bitboard> Board::initBlueDests() {
    unordered_map<Bitboard, Bitboard> result;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            Bitboard current, dest;
            current.set(r * BOARD_SIZE + c);
            if (r > 0) dest.set((r - 1) * BOARD_SIZE + c);
            if (c > 0) dest.set(r * BOARD_SIZE + c - 1);
            if (r > 0 && c > 0) dest.set((r - 1) * BOARD_SIZE + c - 1);
            result.insert(make_pair(current, dest));
        }
    }
    return result;
}

unordered_map<Bitboard, Bitboard> Board::initRedDests() {
    unordered_map<Bitboard, Bitboard> result;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            Bitboard current, dest;
            current.set(r * BOARD_SIZE + c);
            if (r < (BOARD_SIZE - 1)) dest.set((r + 1) * BOARD_SIZE + c);
            if (c < (BOARD_SIZE - 1)) dest.set(r * BOARD_SIZE + c + 1);
            if (r < (BOARD_SIZE - 1) && c < (BOARD_SIZE - 1))
                dest.set((r + 1) * BOARD_SIZE + c + 1);
            result.insert(make_pair(current, dest));
        }
    }
    return result;
}

array<array<size_t, N_CELL>, N_CUBE> Board::initCubePosHashKey() {
    array<array<size_t, N_CELL>, N_CUBE> result;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<unsigned long> randGen;
    for (int i = 0; i < N_CUBE; i++) {
        for (int j = 0; j < N_CELL; j++) result[i][j] = randGen(gen);
    }
    return result;
}

array<size_t, 2> Board::initColorHashKey() {
    array<size_t, 2> result;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<unsigned long> randGen;
    for (int i = 0; i < 2; i++) result[i] = randGen(gen);
    return result;
}

const unordered_map<Bitboard, Bitboard> Board::blueDests = Board::initBlueDests();

const unordered_map<Bitboard, Bitboard> Board::redDests = Board::initRedDests();

const map<tuple<Color, Direction>, Bitboard> Board::moveMasks = Board::initMoveMasks();

const array<array<size_t, N_CELL>, N_CUBE> Board::cubePosHashKey =
    Board::initCubePosHashKey();

const array<size_t, 2> Board::colorHashKey = Board::initColorHashKey();

const Bitboard& Board::getMoveMask(Color nextTurn, Direction dir) {
    return Board::moveMasks.at(make_tuple(nextTurn, dir));
}

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

int Board::getCellByCubeId(int cubeId) const {
    assert(this->bitboards[cubeId].any());
    // TODO: binary search?
    for (int i = 0; i < N_CELL; i++) {
        if (this->bitboards[cubeId].test(i)) return i;
    }
    return -1;
}

// if the board is a terminal board, return the winner
// otherwise, return `None`
Color Board::getWinner() const {
    Bitboard allBlue(0), allRed(0);
    for (int i = 0; i < 6; i++) {
        if (this->bitboards[i].test(0)) return Color::Blue;
        if (this->bitboards[i + 6].test(N_CELL - 1)) return Color::Red;
        allBlue |= this->bitboards[i];
        allRed |= this->bitboards[i + 6];
    }
    if (allBlue.none()) return Color::Red;
    if (allRed.none()) return Color::Blue;
    return Color::None;
}

// generate moves and append the moves to `result`
void Board::generateMoves(vector<Ply>& result, int dice) const {
    pair<int, int> movableCubes = getMovableCubes(dice);
    int movableCubeArr[2] = {movableCubes.first, movableCubes.second};
    Direction directions[] = {
        Direction::Diagonal,
        Direction::Vertical,
        Direction::Horizontal,
    };

    for (int i = 0; i < 2; i++) {
        int cubeId = movableCubeArr[i];
        if (cubeId == -1) continue;

        for (int j = 0; j < 3; j++) {
            Direction& dir = directions[j];
            if ((this->bitboards[cubeId] & Board::getMoveMask(this->nextTurn, dir))
                    .any()) {
                result.push_back(Ply::getPly(cubeId, dir));
            }
        }
    }
}

bitset<12> Board::cubeExist() const {
    bitset<12> result;
    for (int i = 0; i < 12; i++) result.set(i, this->bitboards[i].any());
    return result;
}

void Board::applyPly(Ply& ply) {
    // move the cube
    int oriCell = this->getCellByCubeId(ply.cubeId);
    int newCell = (this->nextTurn == Color::Blue ? oriCell - ply.dir : oriCell + ply.dir);
    this->bitboards[ply.cubeId].reset(oriCell);
    this->bitboards[ply.cubeId].set(newCell);
    this->hash ^= (Board::cubePosHashKey[ply.cubeId][oriCell] ^
                   Board::cubePosHashKey[ply.cubeId][newCell]);

    // remove the cube originally occupying this cell
    for (int i = 0; i < N_CUBE; i++) {
        if (i != ply.cubeId && this->bitboards[i].test(newCell)) {
            this->bitboards[i].reset(newCell);
            this->hash ^= Board::cubePosHashKey[i][newCell];
            break;
        }
    }

    // update nextTurn
    this->nextTurn = (this->nextTurn == Color::Blue ? Color::Red : Color::Blue);
    this->hash ^= (Board::colorHashKey[0] ^ Board::colorHashKey[1]);
}

void Board::getCapturableCubes(vector<int>& result, int cubeId) const {
    if (this->bitboards[cubeId].none()) return;

    Color movingColor = (cubeId < 6 ? Color::Blue : Color::Red);
    const Bitboard& destCandidates =
        (movingColor == Color::Blue ? Board::blueDests.at(this->bitboards[cubeId])
                                    : Board::redDests.at(this->bitboards[cubeId]));
    int oppoCubeStartingId = (movingColor == Color::Blue ? 6 : 0);
    for (int i = oppoCubeStartingId; i < oppoCubeStartingId + 6; i++) {
        if ((this->bitboards[i] & destCandidates).any()) result.push_back(i);
    }
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

pair<int, int> Board::getMovableCubes(int dice) const {
    int cubeIdOffset = (this->nextTurn == Color::Blue ? 0 : 6);
    int exactCubeId = cubeIdOffset + dice - 1;
    if (this->bitboards[exactCubeId].any()) return make_pair(exactCubeId, -1);

    int smallerCubeId = -1, largerCubeId = -1;
    for (int i = exactCubeId - 1; i >= cubeIdOffset; i--) {
        if (this->bitboards[i].any()) {
            smallerCubeId = i;
            break;
        }
    }
    for (int i = exactCubeId + 1; i < (cubeIdOffset + 6); i++) {
        if (this->bitboards[i].any()) {
            largerCubeId = i;
            break;
        }
    }
    return make_pair(smallerCubeId, largerCubeId);
}

void Board::setCube(int cubeId, int position) {
    this->bitboards[cubeId].set(position);
    this->hash ^= Board::cubePosHashKey[cubeId][position];
}

void Board::setNextTurn(Color color) {
    this->nextTurn = color;
    this->hash ^= Board::colorHashKey[color];
}
