#include "solver.hpp"

#include <assert.h>
#include <stdlib.h>

#include <iostream>
using namespace std;

Ply Solver::getBestPly(int dice) {
    vector<Ply> legalPlys;
    this->board.generateMoves(legalPlys, dice);
    assert(!legalPlys.empty());

    // TODO: replace random with NegaScout
    return legalPlys[rand() % legalPlys.size()];
}
