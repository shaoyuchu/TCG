#include "mcts.hpp"

#include <float.h>
#include <math.h>
#include <time.h>

#include <iostream>
#include <thread>

ldbl Node::explorationConst = OPENING_EXPLORATION_CONST;

Node::Node(Board board, Ply& ply, Node* parent) : board(board), ply(ply), parent(parent) {
    this->depth = parent->depth + 1;
}

void Node::deleteChildren() {
    for (Node* child : this->children) {
        child->deleteChildren();
        delete child;
    }
}

void Node::updateCompositeStat() {
    this->cSqrtLogSimCnt = Node::explorationConst * sqrt(log10l(this->simCnt));
    this->sqrtSimCnt = sqrt(this->simCnt);
    this->avgScore = (ldbl)(this->winCnt - this->loseCnt) / (ldbl)(this->simCnt);
    this->updateAmafCompositeStat();
}

void Node::updateAmafCompositeStat() {
    this->amafAvgScore =
        (ldbl)(this->amafWinCnt - this->amafLoseCnt) / (ldbl)(this->amafSimCnt);
}

void Node::expandChildren() {
    vector<Ply>* validPlys = this->board.getAllValidPly();
    for (Ply& ply : (*validPlys)) {
        Node* newChild = new Node(this->board, ply, this);
        newChild->board.applyPly(ply);
        this->children.push_back(newChild);
    }
}

void Node::runSimulation(int trialCnt) {
    int winCnt = 0, loseCnt = 0;
    for (int i = 0; i < trialCnt; i++) {
        Color winner = this->board.getRandomPlayWinner();
        if (winner == Color::Red)
            winCnt++;
        else if (winner == Color::Blue)
            loseCnt++;
    }
    while (1) {
        if (this->simResLock.try_lock()) {
            this->addActualSimRes(trialCnt, winCnt, loseCnt);
            this->simResLock.unlock();
            break;
        }
    }
}

void Node::runSimOnChildren(int trialCnt) {
    int trialPerThread = trialCnt / N_THREAD;
    thread* threads[N_THREAD];

    // simulation
    for (Node* child : this->children) {
        child->simResLock.unlock();
        for (int th = 0; th < N_THREAD; th++) {
            threads[th] = new thread(&Node::runSimulation, child, trialPerThread);
        }
        for (int th = 0; th < N_THREAD; th++) {
            threads[th]->join();
            delete threads[th];
        }
        child->updateCompositeStat();
        this->addActualSimRes(trialCnt, child->winCnt, child->loseCnt);
        child->amafUpdate();
    }
    this->updateCompositeStat();

    // back propagation
    Node* current = this;
    while (current->parent != NULL) {
        current = current->parent;
        current->addActualSimRes(this->simCnt, this->winCnt, this->loseCnt);
        current->updateCompositeStat();
    }
}

void Node::addActualSimRes(int simCnt, int winCnt, int loseCnt) {
    this->simCnt += (ll)simCnt;
    this->winCnt += (ll)winCnt;
    this->loseCnt += (ll)loseCnt;
    this->addVirtualSimRes(simCnt, winCnt, loseCnt);
}

void Node::addVirtualSimRes(int simCnt, int winCnt, int loseCnt) {
    this->amafSimCnt += (ll)simCnt;
    this->amafWinCnt += (ll)winCnt;
    this->amafLoseCnt += (ll)loseCnt;
}

void Node::amafUpdate() {
    bool plyHistory[2][N_ROW][N_COL][6][3] = {0};  // 0: red, 1: blue
    Node* prev = NULL;
    Node* current = this;
    int curNextTurn = (this->board.getNextTurn() == Color::Red ? 0 : 1);
    int cumulativeVirtualSimCnt = 0;
    int cumulativeVirtualWinCnt = 0, cumulativeVirtualLoseCnt = 0;
    while (current != NULL) {
        if (current != this) {
            // update AMAF statistics in children
            for (Node* child : current->children) {
                if (child == prev || child->ply.dir == Direction::None) continue;
                if (plyHistory[curNextTurn][child->ply.row][child->ply.col]
                              [child->ply.num - '0'][static_cast<int>(child->ply.dir)]) {
                    child->addVirtualSimRes(this->simCnt, this->winCnt, this->loseCnt);
                    child->updateAmafCompositeStat();
                    cumulativeVirtualSimCnt += this->simCnt;
                    cumulativeVirtualWinCnt += this->winCnt;
                    cumulativeVirtualLoseCnt += this->loseCnt;
                }
            }
            // collect the added virtual simulation results
            current->addVirtualSimRes(cumulativeVirtualSimCnt, cumulativeVirtualWinCnt,
                                      cumulativeVirtualLoseCnt);
            current->updateAmafCompositeStat();
        }

        // add current ply to plyHistory
        if (current->ply.dir != Direction::None) {
            plyHistory[1 - curNextTurn][current->ply.row][current->ply.col]
                      [current->ply.num - '0'][static_cast<int>(current->ply.dir)] = 1;
        }

        prev = current;
        current = current->parent;
        curNextTurn = 1 - curNextTurn;
    }
}

ldbl Node::getUcb() const {
    ldbl actualScoreRatio =
        min((ldbl)1, (ldbl)this->simCnt * (ldbl)RAVE_RATIO_DECAY_RATE);
    ldbl scoreTerm = actualScoreRatio * this->avgScore +
                     ((ldbl)1 - actualScoreRatio) * this->amafAvgScore;
    ldbl explorationTerm = (this->parent->cSqrtLogSimCnt / this->sqrtSimCnt);

    // parent: max node
    if (this->depth % 2) {
        return scoreTerm + explorationTerm;
    }
    // parent: min node
    return -scoreTerm + explorationTerm;
}

void Node::expandAndRunSim(int trialCnt) {
    // terminal position: directly add the simulation result
    if (this->board.isCompleted()) {
        Color winner = this->board.getWinner();
        if (winner == Color::Red) {
            this->addActualSimRes(trialCnt, trialCnt, 0);
        } else if (winner == Color::Blue) {
            this->addActualSimRes(trialCnt, 0, trialCnt);
        } else if (winner == Color::Empty) {
            this->addActualSimRes(trialCnt, 0, 0);
        }
        this->updateCompositeStat();

        // back propagation
        Node* current = this;
        while (current->parent != NULL) {
            current = current->parent;
            current->addActualSimRes(this->simCnt, this->winCnt, this->loseCnt);
            current->updateCompositeStat();
        }
        return;
    }

    this->expandChildren();
    this->runSimOnChildren(trialCnt);
}

Node* Node::getChildWithLargestUcb() const {
    Node* bestChild = this->children[0];
    ldbl bestUcb = this->children[0]->getUcb();
    for (Node* child : this->children) {
        ldbl ucb = child->getUcb();
        if (ucb > bestUcb) {
            bestUcb = ucb;
            bestChild = child;
        }
    }
    return bestChild;
}

ostream& operator<<(ostream& os, Node node) {
    os << "{" << node.ply << "} [" << node.depth << "] Win: " << node.winCnt
       << " Lose: " << node.loseCnt << " Total Simulation: " << node.simCnt
       << " Average Score: " << node.avgScore;
    return os;
}

MCTS::MCTS(Board board) {
    this->root = new Node(board);
    this->nodeCnt = 1;
    this->leafCnt = 1;
    int totalDistanceToCorner = this->root->board.getTotalDistanceToCorner();
    if (totalDistanceToCorner < TOTAL_DISTANCE_THRES1 &&
        totalDistanceToCorner >= TOTAL_DISTANCE_THRES2)
        Node::explorationConst = MIDDLE_EXPLORATION_CONST;
    else if (totalDistanceToCorner < TOTAL_DISTANCE_THRES2)
        Node::explorationConst = ENDING_EXPLORATION_CONST;
}

MCTS::~MCTS() {
    cerr << this->nodeCnt << "," << this->root->simCnt << "," << this->root->avgScore
         << "," << this->root->amafSimCnt << "," << this->root->amafAvgScore << ","
         << (double)this->totalLeafDepth / (double)this->leafCnt << "," << this->maxDepth
         << "," << this->root->board.getTotalDistanceToCorner() << endl;

    if (this->root != NULL) {
        this->root->deleteChildren();
        delete this->root;
    }
}

Node* MCTS::selectPV() const {
    Node* current = this->root;
    while (!current->children.empty()) {
        current = current->getChildWithLargestUcb();
    }
    return current;
}

Node* MCTS::getMaxAvgScoreDep1Node() const {
    vector<Node*> children = this->root->children;
    ldbl bestAvgScore = children[0]->avgScore;
    Node* bestChild = children[0];
    for (Node* child : children) {
        ldbl avgScore = child->avgScore;
        if (avgScore > bestAvgScore) {
            bestAvgScore = avgScore;
            bestChild = child;
        }
    }
    return bestChild;
}

Ply& MCTS::getBestPly(double timeLimitInSec) {
    struct timespec startTime, endTime;
    double executedTime = 0;
    clock_gettime(CLOCK_REALTIME, &startTime);

    // generate all valid plys
    this->root->expandAndRunSim(N_TRIAL_PER_SIM);
    if (this->root->children.size() == 1) {
        return this->root->children[0]->ply;
    }
    do {
        Node* pvLeaf = this->selectPV();
        pvLeaf->expandAndRunSim(N_TRIAL_PER_SIM);

        // update statistics
        int pvDepth = pvLeaf->depth;
        int newExpandedNodeCnt = pvLeaf->children.size();
        this->nodeCnt += newExpandedNodeCnt;
        this->leafCnt += newExpandedNodeCnt - 1;
        this->totalLeafDepth -= pvDepth;
        this->totalLeafDepth += (pvDepth + 1) * newExpandedNodeCnt;
        this->maxDepth = max(this->maxDepth, pvDepth + 1);

        // update executedTime
        clock_gettime(CLOCK_REALTIME, &endTime);
        executedTime = (double)(endTime.tv_sec - startTime.tv_sec) +
                       (double)(endTime.tv_nsec - startTime.tv_nsec) * 1e-9;
    } while (executedTime < timeLimitInSec);

    // pick the ply with the highest average score
    Node* maxAvgScoreDep1Node = this->getMaxAvgScoreDep1Node();
    return maxAvgScoreDep1Node->ply;
}

PureMonteCarlo::~PureMonteCarlo() { cerr << this->simCnt << endl; }

Ply& PureMonteCarlo::getBestPly(double timeLimitInSec) {
    struct timespec startTime, endTime;
    double executedTime = 0;
    clock_gettime(CLOCK_REALTIME, &startTime);

    vector<Ply>* validPlys = board.getAllValidPly();
    array<int, 18> totalScore = {0};
    int simCntPerRound = N_TRIAL_PER_SIM * validPlys->size();
    if (validPlys->size() == 1) return validPlys->at(0);
    do {
        // run simulation
        for (int i = 0; i < validPlys->size(); i++) {
            Board copiedBoard(board);
            copiedBoard.applyPly(validPlys->at(i));
            for (int simId = 0; simId < N_TRIAL_PER_SIM; simId++) {
                Color winner = copiedBoard.getRandomPlayWinner();
                if (winner == Color::Red)
                    totalScore[i] += 1;
                else if (winner == Color::Blue)
                    totalScore[i] -= 1;
            }
        }
        this->simCnt += simCntPerRound;

        // update executedTime
        clock_gettime(CLOCK_REALTIME, &endTime);
        executedTime = (double)(endTime.tv_sec - startTime.tv_sec) +
                       (double)(endTime.tv_nsec - startTime.tv_nsec) * 1e-9;
    } while (executedTime < timeLimitInSec);

    // get the ply with the highest score
    int highestScore = totalScore[0];
    Ply& bestPly = validPlys->at(0);
    for (int i = 1; i < validPlys->size(); i++) {
        if (totalScore[i] > highestScore) {
            highestScore = totalScore[i];
            bestPly = validPlys->at(i);
        }
    }
    return bestPly;
}