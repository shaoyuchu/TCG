#include "solver.hpp"

#include <assert.h>

Ply Solver::getBestPly(int dice) {
    vector<Ply> validPlys;
    this->board.generateMove(validPlys, dice);
    assert(!validPlys.empty());
    // TODO: replace random with NegaScout
    return validPlys[0];
}
