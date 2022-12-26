#include "solver.hpp"

#include <assert.h>
#include <stdlib.h>

#include <iostream>
using namespace std;

map<array<bool, 6>, array<int, 6>> Solver::initCubeCoverages() {
    map<array<bool, 6>, array<int, 6>> result;
    array<bool, 6> empty;
    Solver::initCubeCoverages(empty, 0, result);
    return result;
}

void Solver::initCubeCoverages(array<bool, 6> current, int next,
                               map<array<bool, 6>, array<int, 6>>& result) {
    if (next == 5) {
        array<bool, 6> cubeExist(current);
        cubeExist[5] = true;
        result.insert(make_pair(cubeExist, Solver::computeCubeCoverage(cubeExist)));
        cubeExist[5] = false;
        result.insert(make_pair(cubeExist, Solver::computeCubeCoverage(cubeExist)));
    } else {
        current[next] = true;
        initCubeCoverages(current, next + 1, result);
        current[next] = false;
        initCubeCoverages(current, next + 1, result);
    }
}

array<int, 6> Solver::computeCubeCoverage(array<bool, 6>& current) {
    array<int, 6> result = {0};
    int zeroCount = 0;
    for (int i = 0; i < 6; i++) {
        if (current[i]) result[i] += (zeroCount + 1);
        zeroCount = (current[i] ? 0 : zeroCount + 1);
    }
    zeroCount = 0;
    for (int i = 5; i >= 0; i--) {
        if (current[i]) result[i] += zeroCount;
        zeroCount = (current[i] ? 0 : zeroCount + 1);
    }
    return result;
}

const map<array<bool, 6>, array<int, 6>> Solver::cubeCoverage =
    Solver::initCubeCoverages();

int Solver::evalAbsPos(const Board& board) {
    array<bool, 12> cubeExist = board.cubeExist();

    // blue
    for (int i = 0; i < 6; i++) {
    }

    // red
    for (int i = 6; i < 12; i++) {
    }
    return 0;
}

int Solver::evalThreat(const Board& board) { return 0; }

int Solver::evaluateBoard(const Board& board) {
    return this->evalAbsPos(board) - this->evalThreat(board);
}

Ply Solver::getBestPly(int dice) {
    vector<Ply> legalPlys;
    this->board.generateMoves(legalPlys, dice);
    assert(!legalPlys.empty());

    // TODO: replace random with NegaScout
    return legalPlys[rand() % legalPlys.size()];
}
