#include "solver.hpp"

#include <stdlib.h>

#include <cfloat>
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

double Solver::evaluateBoard(const Board& board) {
    bitset<12> cubeExist = board.cubeExist();
    bitset<12> blueCubeExist = cubeExist & bitset<12>(63);
    bitset<12> redCubeExist = cubeExist >> 6;
    array<int, 6> blueCubeCoverage = Solver::cubeCoverage.at(blueCubeExist);
    array<int, 6> redCubeCoverage = Solver::cubeCoverage.at(redCubeExist);
    int pieceValues[12] = {0};

    double blueVal = 0, redVal = 0;
    // blue
    for (int i = 0; i < 6; i++) {
        if (blueCubeCoverage[i] == 0) continue;
        int pieceVal =
            2 << (4 - Solver::dist2TargetCorner[N_CELL - 1 - board.getCellByCubeId(i)]);
        blueVal += (pieceVal * blueCubeCoverage[i]);
        pieceValues[i] = pieceVal;
    }
    // red
    for (int i = 0; i < 6; i++) {
        if (redCubeCoverage[i] == 0) continue;
        int pieceVal = 2 << (4 - Solver::dist2TargetCorner[board.getCellByCubeId(i + 6)]);
        redVal += (pieceVal * redCubeCoverage[i]);
        pieceValues[i + 6] = pieceVal;
    }

    double threatVal = 0;
    int oppoCubeStartingId = (this->nextTurn == Color::Blue ? 6 : 0);
    for (int i = oppoCubeStartingId; i < oppoCubeStartingId + 6; i++) {
        vector<int> capturableCubes;
        board.getCapturableCubes(capturableCubes, i);
        int maxPieceVal = 0;
        for (int& capturableCube : capturableCubes) {
            if ((this->nextTurn == Color::Blue && capturableCube < 6) ||
                (this->nextTurn == Color::Red && capturableCube >= 6)) {
                maxPieceVal = max(maxPieceVal, pieceValues[capturableCube]);
            }
        }
        threatVal += maxPieceVal;
    }

    return (this->nextTurn == Color::Blue
                ? ATTACK_FACTOR * blueVal - redVal - THREAT_FACTOR * threatVal
                : ATTACK_FACTOR * redVal - blueVal - THREAT_FACTOR * threatVal);
}

Ply Solver::getBestPly(int dice) {
    vector<Ply> legalPlys;
    this->baseBoard.generateMoves(legalPlys, dice);
    if (legalPlys.size() == 1) return legalPlys[0];

    double maxScore = -DBL_MAX;
    Ply& bestPly = legalPlys[0];
    for (Ply& ply : legalPlys) {
        Board newBoard(this->baseBoard);
        newBoard.applyPly(ply);
        double newBoardScore = -this->star0(newBoard, -DBL_MAX, DBL_MAX, 0, MAX_DEPTH);
        if (newBoardScore > maxScore) {
            maxScore = newBoardScore;
            bestPly = ply;
        }
    }
    return bestPly;
}

double Solver::star0(Board board, double alpha, double beta, int currentDepth,
                     int remainingDepth) {
    double total = 0;
    for (int dice = 1; dice <= 6; dice++) {
        total += -negaScout(board, dice, -DBL_MAX, DBL_MAX, currentDepth, remainingDepth);
    }
    return (total / (double)6);
}

double Solver::negaScout(Board board, int dice, double alpha, double beta,
                         int currentDepth, int remainingDepth) {
    // TODO: time control
    // terminal node, remaining depth = 0
    if (board.getWinner() != Color::None || remainingDepth == 0) {
        double utility = this->evaluateBoard(board);
        return (currentDepth % 2 ? -utility : utility);
    }

    vector<Ply> legalPlys;
    board.generateMoves(legalPlys, dice);
    double lowerBound = -DBL_MAX, upperBound = beta;
    for (Ply& ply : legalPlys) {
        Board newBoard(board);
        newBoard.applyPly(ply);
        double searchResult = this->star0(newBoard, max(alpha, lowerBound), upperBound,
                                          currentDepth + 1, remainingDepth - 1);
        if (searchResult > lowerBound) {
            lowerBound = (upperBound == beta || searchResult >= beta)
                             ? searchResult
                             : this->star0(newBoard, searchResult, beta, currentDepth + 1,
                                           remainingDepth - 1);
        }
        if (lowerBound == DBL_MAX || lowerBound >= beta) return lowerBound;
        upperBound = max(alpha, lowerBound) + 1;
    }
    return lowerBound;
}
