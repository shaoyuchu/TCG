#include "solver.hpp"

#include <stdlib.h>

#include <cfloat>
#include <iostream>
using namespace std;

int Solver::getCount = 0;

const unordered_map<bitset<12>, array<int, 6>> Solver::cubeMobility =
    Solver::initCubeCoverages();

const array<int, N_CELL> Solver::dist2TargetCorner = {
    4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 4, 3, 2, 2, 2, 4, 3, 2, 1, 1, 4, 3, 2, 1, 0};

array<double, MAX_TOTAL_DEPTH> Solver::totalTimeUsage = {0};

array<int, MAX_TOTAL_DEPTH> Solver::totalCallCnt = {0};

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
    array<int, 6> blueCubeMobility = Solver::cubeMobility.at(blueCubeExist);
    array<int, 6> redCubeMobility = Solver::cubeMobility.at(redCubeExist);
    int pieceValues[12] = {0};

    double blueVal = 0, redVal = 0;
    for (int i = 0; i < 6; i++) {
        if (blueCubeMobility[i] == 0) continue;
        int pieceVal =
            2 << (4 - Solver::dist2TargetCorner[N_CELL - 1 - board.getCellByCubeId(i)]);
        blueVal += (pieceVal * blueCubeMobility[i]);
        pieceValues[i] = pieceVal;
    }
    for (int i = 0; i < 6; i++) {
        if (redCubeMobility[i] == 0) continue;
        int pieceVal = 2 << (4 - Solver::dist2TargetCorner[board.getCellByCubeId(i + 6)]);
        redVal += (pieceVal * redCubeMobility[i]);
        pieceValues[i + 6] = pieceVal;
    }

    double threatVal = 0;
    int oppoCubeIdOffset = (this->nextTurn == Color::Blue ? 6 : 0);
    array<int, 6>& oppoCoverage =
        (this->nextTurn == Color::Blue ? blueCubeMobility : redCubeMobility);
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

Ply Solver::getBestPly(int dice, double remainingSec) {
    Solver::getCount++;
    vector<Ply> legalPlys;
    this->baseBoard.generateMoves(legalPlys, dice);
    for (Ply& ply : legalPlys) cerr << ply.toString() << endl;
    if (legalPlys.size() == 1) return legalPlys[0];

    // first round
    int curDepBestPlyId = 0;
    int curDepBestScore = MIN_EVAL;
    double scores[6];
    Board nextBoard[6];
    struct timespec startTime, endTime;
    for (int i = 0; i < legalPlys.size(); i++) {
        nextBoard[i] = this->baseBoard;
        nextBoard[i].applyPly(legalPlys[i]);
        if (nextBoard[i].getWinner() == this->nextTurn) return legalPlys[i];

        clock_gettime(CLOCK_REALTIME, &startTime);
        scores[i] = this->star1Max(nextBoard[i], -DBL_MAX, DBL_MAX, MIN_DEPTH);
        if (scores[i] > curDepBestScore) {
            curDepBestScore = scores[i];
            curDepBestPlyId = i;
        }
        clock_gettime(CLOCK_REALTIME, &endTime);
        Solver::totalTimeUsage[MIN_DEPTH] +=
            (double)(endTime.tv_sec - startTime.tv_sec) +
            (double)(endTime.tv_nsec - startTime.tv_nsec) * 1e-9;
        Solver::totalCallCnt[MIN_DEPTH] += 1;
    }
    cerr << "Dep" << MIN_DEPTH << ":";
    for (int i = 0; i < legalPlys.size(); i++) cerr << " " << scores[i];
    cerr << endl;
    int bestPlyId = curDepBestPlyId;

    int nextDepth = MIN_DEPTH + 2;
    int maxDepth =
        (Solver::getCount <= OPEN_PLY_COUNT
             ? OPEN_MAX_DEPTH
             : (Solver::getCount <= (OPEN_PLY_COUNT + MIDDLE_PLY_COUNT) ? MIDDLE_MAX_DEPTH
                                                                        : END_MAX_DEPTH));
    while (nextDepth <= maxDepth && (Solver::totalCallCnt[nextDepth] == 0 ||
                                     remainingSec > ((Solver::totalTimeUsage[nextDepth] /
                                                      Solver::totalCallCnt[nextDepth]) *
                                                     (double)legalPlys.size()))) {
        curDepBestScore = MIN_EVAL;
        for (int i = 0; i < legalPlys.size(); i++) {
            clock_gettime(CLOCK_REALTIME, &startTime);
            double newScore = this->star1Max(nextBoard[i], scores[i] - IDAS_THRES,
                                             scores[i] + IDAS_THRES, nextDepth);
            if (newScore <= scores[i] - IDAS_THRES) {
                newScore = this->star1Max(nextBoard[i], -DBL_MAX, newScore, nextDepth);
            } else if (newScore >= scores[i] + IDAS_THRES) {
                newScore = this->star1Max(nextBoard[i], newScore, DBL_MAX, nextDepth);
            }
            scores[i] = newScore;
            if (scores[i] > curDepBestScore) {
                curDepBestScore = scores[i];
                curDepBestPlyId = i;
            }
            clock_gettime(CLOCK_REALTIME, &endTime);
            Solver::totalTimeUsage[nextDepth] +=
                (double)(endTime.tv_sec - startTime.tv_sec) +
                (double)(endTime.tv_nsec - startTime.tv_nsec) * 1e-9;
            Solver::totalCallCnt[nextDepth] += 1;
        }
        cerr << "Dep" << nextDepth << ":";
        for (int i = 0; i < legalPlys.size(); i++) cerr << " " << scores[i];
        cerr << endl;
        nextDepth += 2;
        bestPlyId = curDepBestPlyId;
    }
    return legalPlys[bestPlyId];
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

tuple<int, double, double, double> Solver::lookupTp(Board& board, int dice) {
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
