#include "solver.hpp"

#include <assert.h>
#include <stdlib.h>

#include <iostream>
using namespace std;

const unordered_map<bitset<12>, array<int, 6>> Solver::cubeCoverage =
    Solver::initCubeCoverages();

const array<int, N_CELL> Solver::dist2TargetCorner = {
    4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 4, 3, 2, 2, 2, 4, 3, 2, 1, 1, 4, 3, 2, 1, 0};

unordered_map<bitset<12>, array<int, 6>> Solver::initCubeCoverages() {
    unordered_map<bitset<12>, array<int, 6>> result;
    bitset<12> empty(0);
    Solver::initCubeCoverages(empty, 0, result);
    return result;
}

void Solver::initCubeCoverages(bitset<12> current, int next,
                               unordered_map<bitset<12>, array<int, 6>>& result) {
    if (next == 5) {
        bitset<12> cubeExist(current);
        cubeExist.set(5, true);
        result.insert(make_pair(cubeExist, Solver::computeCubeCoverage(cubeExist)));
        cubeExist.set(5, false);
        result.insert(make_pair(cubeExist, Solver::computeCubeCoverage(cubeExist)));
    } else {
        current.set(next, true);
        initCubeCoverages(current, next + 1, result);
        current.set(next, false);
        initCubeCoverages(current, next + 1, result);
    }
}

array<int, 6> Solver::computeCubeCoverage(bitset<12>& current) {
    array<int, 6> result = {0};
    int zeroCount = 0;
    for (int i = 0; i < 6; i++) {
        if (current.test(i)) result[i] += (zeroCount + 1);
        zeroCount = (current.test(i) ? 0 : zeroCount + 1);
    }
    zeroCount = 0;
    for (int i = 5; i >= 0; i--) {
        if (current.test(i)) result[i] += zeroCount;
        zeroCount = (current.test(i) ? 0 : zeroCount + 1);
    }
    return result;
}

int Solver::evalAbsPos(const Board& board) {
    bitset<12> cubeExist = board.cubeExist();
    bitset<12> blueCubeExist = cubeExist & bitset<12>(63);
    bitset<12> redCubeExist = cubeExist >> 6;
    array<int, 6> blueCubeCoverage = Solver::cubeCoverage.at(blueCubeExist);
    array<int, 6> redCubeCoverage = Solver::cubeCoverage.at(redCubeExist);

    int blueVal = 0, redVal = 0;
    // blue
    for (int i = 0; i < 6; i++) {
        if (blueCubeCoverage[i] == 0) continue;
        int pieceVal =
            1 << (4 - Solver::dist2TargetCorner[N_CELL - 1 - board.getCellByCubeId(i)]);
        blueVal += (pieceVal * blueCubeCoverage[i]);
    }
    // red
    for (int i = 0; i < 6; i++) {
        if (redCubeCoverage[i] == 0) continue;
        int pieceVal = 1 << (4 - Solver::dist2TargetCorner[board.getCellByCubeId(i + 6)]);
        redVal += (pieceVal * redCubeCoverage[i]);
    }
    return (board.getNextTurn() == Color::Blue ? blueVal - redVal : redVal - blueVal);
}

int Solver::evalThreat(const Board& board) { return 0; }

int Solver::evaluateBoard(const Board& board) {
    return this->evalAbsPos(board) - this->evalThreat(board);
}

Ply Solver::getBestPly(Board& board, int dice) {
    vector<Ply> legalPlys;
    board.generateMoves(legalPlys, dice);
    assert(!legalPlys.empty());
    this->evalAbsPos(board);

    // TODO: replace random with NegaScout
    return legalPlys[rand() % legalPlys.size()];
}
