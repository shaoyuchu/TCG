#include "solver.hpp"

#include <stdlib.h>

#include <cfloat>
#include <iostream>
using namespace std;

const unordered_map<bitset<12>, array<int, 6>> Solver::cubeCoverage =
    Solver::initCubeCoverages();

const array<int, N_CELL> Solver::dist2TargetCorner = {
    4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 4, 3, 2, 2, 2, 4, 3, 2, 1, 1, 4, 3, 2, 1, 0};

unordered_map<size_t, tuple<int, double, double, double>> Solver::transpositionTable =
    unordered_map<size_t, tuple<int, double, double, double>>();

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
    int oppoCubeIdOffset = (this->nextTurn == Color::Blue ? 6 : 0);
    array<int, 6>& oppoCoverage =
        (this->nextTurn == Color::Blue ? blueCubeCoverage : redCubeCoverage);
    for (int i = 0; i < 6; i++) {
        vector<int> capturableCubes;
        board.getCapturableCubes(capturableCubes, i + oppoCubeIdOffset);
        int maxPieceVal = 0;
        for (int& capturableCube : capturableCubes) {
            if (pieceValues[capturableCube] > maxPieceVal)
                maxPieceVal = pieceValues[capturableCube];
        }
        threatVal += (double)(maxPieceVal * oppoCoverage[i]);
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
        double newBoardScore = this->star1Max(newBoard, -DBL_MAX, DBL_MAX, MAX_DEPTH);
        if (newBoardScore > maxScore) {
            maxScore = newBoardScore;
            bestPly = ply;
        }
    }
    return bestPly;
}

double Solver::star0Max(Board& board, double alpha, double beta, int depth) {
    double total = 0;
    for (int dice = 1; dice <= 6; dice++)
        total += negaScoutMin(board, dice, -DBL_MAX, DBL_MAX, depth);
    return (total / (double)6);
}

double Solver::star0Min(Board& board, double alpha, double beta, int depth) {
    double total = 0;
    for (int dice = 1; dice <= 6; dice++)
        total += negaScoutMax(board, dice, -DBL_MAX, DBL_MAX, depth);
    return (total / (double)6);
}

tuple<int, double, double, double> Solver::lookupTp(Board& board,
                                                    int dice) throw(out_of_range) {
    return Solver::transpositionTable.at(board.getHash(dice));
}

double Solver::star05Max(Board& board, double alpha, double beta, int depth) {
    double total = 0;
    double lowerBound = MIN_EVAL, upperBound = MAX_EVAL;
    for (int dice = 1; dice <= 6; dice++) {
        double score = negaScoutMin(board, dice, MIN_EVAL, MAX_EVAL, depth);
        lowerBound += (score - MIN_EVAL) / (double)6;
        upperBound += (score - MAX_EVAL) / (double)6;
        if (lowerBound >= beta) return lowerBound;
        if (upperBound <= alpha) return upperBound;
        total += score;
    }
    return (total / (double)6);
}

double Solver::star05Min(Board& board, double alpha, double beta, int depth) {
    double total = 0;
    double lowerBound = MIN_EVAL, upperBound = MAX_EVAL;
    for (int dice = 1; dice <= 6; dice++) {
        double score = negaScoutMax(board, dice, MIN_EVAL, MAX_EVAL, depth);
        lowerBound += (score - MIN_EVAL) / (double)6;
        upperBound += (score - MAX_EVAL) / (double)6;
        if (lowerBound >= beta) return lowerBound;
        if (upperBound <= alpha) return upperBound;
        total += score;
    }
    return (total / (double)6);
}

double Solver::star1Max(Board& board, double alpha, double beta, int depth) {
    double total = 0;
    double windowLeft = (double)6 * (alpha - MAX_EVAL) + MAX_EVAL;
    double windowRight = (double)6 * (beta - MIN_EVAL) + MIN_EVAL;
    double lowerBound = MIN_EVAL, upperBound = MAX_EVAL;
    for (int dice = 1; dice <= 6; dice++) {
        double score = negaScoutMin(board, dice, max(windowLeft, MIN_EVAL),
                                    min(windowRight, MAX_EVAL), depth);
        lowerBound += (score - MIN_EVAL) / (double)6;
        upperBound += (score - MAX_EVAL) / (double)6;
        if (score >= windowRight) return lowerBound;
        if (score <= windowLeft) return upperBound;
        total += score;
        windowLeft += MAX_EVAL - score;
        windowRight += MIN_EVAL - score;
    }
    return (total / (double)6);
}

double Solver::star1Min(Board& board, double alpha, double beta, int depth) {
    double total = 0;
    double windowLeft = (double)6 * (alpha - MAX_EVAL) + MAX_EVAL;
    double windowRight = (double)6 * (beta - MIN_EVAL) + MIN_EVAL;
    double lowerBound = MIN_EVAL, upperBound = MAX_EVAL;
    for (int dice = 1; dice <= 6; dice++) {
        double score = negaScoutMax(board, dice, max(windowLeft, MIN_EVAL),
                                    min(windowRight, MAX_EVAL), depth);
        lowerBound += (score - MIN_EVAL) / (double)6;
        upperBound += (score - MAX_EVAL) / (double)6;
        if (score >= windowRight) return lowerBound;
        if (score <= windowLeft) return upperBound;
        total += score;
        windowLeft += MAX_EVAL - score;
        windowRight += MIN_EVAL - score;
    }
    return (total / (double)6);
}

double Solver::negaScoutMax(Board& board, int dice, double alpha, double beta,
                            int depth) {
    if (board.getWinner() != Color::None || depth == 0) {
        return this->evaluateBoard(board);
    }
    try {
        tuple<int, double, double, double> tpFound = this->lookupTp(board, dice);
        int tDepth = get<0>(tpFound);
        double tAlpha = get<1>(tpFound), tBeta = get<2>(tpFound);
        double tValue = get<3>(tpFound);
        if (depth <= tDepth) {
            if (tAlpha < tValue && tValue < tBeta) {
                return tValue;
            } else if (tValue >= tBeta) {
                // beta cut, lower bound
                alpha = max(alpha, tValue);
                if (alpha >= beta) return alpha;
            } else {
                // alpha cut, upper bound
                beta = min(beta, tValue);
                if (beta <= alpha) return beta;
            }
        }
    } catch (const out_of_range& except) {
    }

    vector<Ply> legalPlys;
    board.generateMoves(legalPlys, dice);
    Board firstNewBoard(board);
    firstNewBoard.applyPly(legalPlys[0]);
    double lowerBound = this->star1Max(firstNewBoard, alpha, beta, depth - 1);
    if (lowerBound < beta) {
        for (int i = 1; i < legalPlys.size(); i++) {
            Board newBoard(board);
            newBoard.applyPly(legalPlys[i]);
            double searchResult =
                this->star1Max(newBoard, lowerBound, lowerBound + 1, depth - 1);
            if (searchResult > lowerBound) {
                lowerBound =
                    (searchResult >= beta
                         ? searchResult
                         : this->star1Max(newBoard, searchResult, beta, depth - 1));
            }
            if (lowerBound >= beta) break;
        }
    }
    Solver::insertTp(board, dice, depth, alpha, beta, lowerBound);
    return lowerBound;
}

void Solver::insertTp(Board& board, int dice, int depth, double alpha, double beta,
                      double value) {
    Solver::transpositionTable.insert(
        make_pair(board.getHash(dice), make_tuple(depth, alpha, beta, value)));
}

double Solver::negaScoutMin(Board& board, int dice, double alpha, double beta,
                            int depth) {
    if (board.getWinner() != Color::None || depth == 0) {
        return this->evaluateBoard(board);
    }
    try {
        tuple<int, double, double, double> tpFound = this->lookupTp(board, dice);
        int tDepth = get<0>(tpFound);
        double tAlpha = get<1>(tpFound), tBeta = get<2>(tpFound);
        double tValue = get<3>(tpFound);
        if (depth <= tDepth) {
            if (tAlpha < tValue && tValue < tBeta) {
                return tValue;
            } else if (tValue >= tBeta) {
                // beta cut, lower bound
                alpha = max(alpha, tValue);
                if (alpha >= beta) return alpha;
            } else {
                // alpha cut, upper bound
                beta = min(beta, tValue);
                if (beta <= alpha) return beta;
            }
        }
    } catch (const out_of_range& except) {
    }

    vector<Ply> legalPlys;
    board.generateMoves(legalPlys, dice);
    Board firstNewBoard(board);
    firstNewBoard.applyPly(legalPlys[0]);
    double upperBound = this->star1Min(firstNewBoard, alpha, beta, depth - 1);
    if (upperBound > alpha) {
        for (int i = 1; i < legalPlys.size(); i++) {
            Board newBoard(board);
            newBoard.applyPly(legalPlys[i]);
            double searchResult =
                this->star1Min(newBoard, upperBound - 1, upperBound, depth - 1);
            if (searchResult < upperBound) {
                upperBound =
                    (searchResult <= alpha
                         ? searchResult
                         : this->star1Min(newBoard, alpha, searchResult, depth - 1));
            }
            if (upperBound <= alpha) break;
        }
    }
    Solver::insertTp(board, dice, depth, alpha, beta, upperBound);
    return upperBound;
}
