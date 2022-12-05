#include "mcts.hpp"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <time.h>

#include <iostream>

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
    this->cSqrtLogSimCnt = (ldbl)EXPLORATION_CONST * sqrt(log10l(this->simCnt));
    this->sqrtSimCnt = sqrt(this->simCnt);
    this->avgScore = (ldbl)(this->winCnt - this->loseCnt) / (ldbl)(this->simCnt);
    this->updateAmafCompositeStat();
}

void Node::updateAmafCompositeStat() {
    this->amafCSqrtLogSimCnt = (ldbl)EXPLORATION_CONST * sqrt(log10l(this->amafSimCnt));
    this->amafSqrtSimCnt = sqrt(this->amafSimCnt);
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

void Node::runSimOnChildren(int trialCnt) {
    // simulation
    for (Node* child : this->children) {
        for (int i = 0; i < trialCnt; i++) {
            Color winner = child->board.getRandomPlayWinner();
            if (winner == Color::Red)
                child->addActualSimRes(1, 1, 0);
            else if (winner == Color::Blue)
                child->addActualSimRes(1, 0, 1);
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
    this->simCnt += simCnt;
    this->winCnt += winCnt;
    this->loseCnt += loseCnt;
    this->addVirtualSimRes(simCnt, winCnt, loseCnt);
}

void Node::addVirtualSimRes(int simCnt, int winCnt, int loseCnt) {
    this->amafSimCnt += simCnt;
    this->amafWinCnt += winCnt;
    this->amafLoseCnt += loseCnt;
}

void Node::amafUpdate() {
    bool plyHistory[2][N_ROW][N_COL][6][3] = {0};  // 0: red, 1: blue
    Node* prev = NULL;
    Node* current = this;
    int curNextTurn = (this->board.getNextTurn() == Color::Red ? 0 : 1);
    while (current != NULL) {
        // update AMAF statistics in children
        for (Node* child : current->children) {
            if (child == prev || child->ply.dir == Direction::None) continue;
            if (plyHistory[curNextTurn][child->ply.row][child->ply.col]
                          [child->ply.num - '0'][static_cast<int>(child->ply.dir)]) {
                child->addVirtualSimRes(this->simCnt, this->winCnt, this->loseCnt);
                child->updateAmafCompositeStat();
            }
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
    ldbl actualScore = 0, amafScore = 0;
    ldbl actualScoreRatio =
        min((ldbl)1, (ldbl)this->simCnt * (ldbl)RAVE_RATIO_DECAY_RATE);
    if (this->depth % 2) {
        // parent: max node
        actualScore = this->avgScore + (this->parent->cSqrtLogSimCnt / this->sqrtSimCnt);
        amafScore = this->amafAvgScore +
                    (this->parent->amafCSqrtLogSimCnt / this->amafSqrtSimCnt);
    } else {
        // parent: min node
        actualScore = (ldbl)0 - this->avgScore;
        amafScore = (ldbl)0 - this->amafAvgScore;
    }
    return actualScoreRatio * actualScore + ((ldbl)1 - actualScoreRatio) * amafScore;
}

void Node::expandAndRunSim(int trialCnt) {
    this->expandChildren();
    this->runSimOnChildren(trialCnt);
}

Node* Node::getChildWithLargestUcb() const {
    assert(!this->children.empty());
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
}

MCTS::~MCTS() {
    cerr << this->nodeCnt << "," << (double)this->totalLeafDepth / (double)this->leafCnt
         << "," << this->maxDepth << endl;

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