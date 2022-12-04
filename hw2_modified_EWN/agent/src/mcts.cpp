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

void Node::updateCSqrtLogSimCnt() {
    this->cSqrtLogSimCnt = (ldbl)EXPLORATION_CONST * sqrt(log10l(this->simCnt));
}

void Node::updateSqrtSimCnt() { this->sqrtSimCnt = sqrt(this->simCnt); }

void Node::updateAvgScore() {
    this->avgScore = (ldbl)(this->winCnt - this->loseCnt) / (ldbl)(this->simCnt);
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
                child->winCnt += 1;
            else if (winner == Color::Blue)
                child->loseCnt += 1;
        }
        child->simCnt = trialCnt;
        child->updateCSqrtLogSimCnt();
        child->updateSqrtSimCnt();
        child->updateAvgScore();
        this->simCnt += trialCnt;
        this->winCnt += child->winCnt;
        this->loseCnt += child->loseCnt;
    }
    this->updateCSqrtLogSimCnt();
    this->updateSqrtSimCnt();
    this->updateAvgScore();

    // back propagation
    Node* current = this;
    while (current->parent != NULL) {
        current = current->parent;
        current->simCnt += this->simCnt;
        current->winCnt += this->winCnt;
        current->loseCnt += this->loseCnt;
        current->updateCSqrtLogSimCnt();
        current->updateSqrtSimCnt();
        current->updateAvgScore();
    }
}

ldbl Node::getUcb() const {
    // parent: max node
    if (this->depth % 2)
        return this->avgScore + (this->parent->cSqrtLogSimCnt / this->sqrtSimCnt);

    // parent: min node
    return (ldbl)0 - this->avgScore;
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
    os << "{" << node.getPly() << "} [" << node.getDepth()
       << "] Win: " << node.getWinCnt() << " Lose: " << node.getLoseCnt()
       << " Total Simulation: " << node.getSimCnt()
       << " Average Score: " << node.getAvgScore();
    return os;
}

MCTS::~MCTS() {
    if (this->root != NULL) {
        this->root->deleteChildren();
        delete this->root;
    }
}

Node* MCTS::selectPV() const {
    Node* current = this->root;
    while (!current->getChildren().empty()) {
        current = current->getChildWithLargestUcb();
    }
    return current;
}

Node* MCTS::getMaxAvgScoreDep1Node() const {
    vector<Node*> children = this->root->getChildren();
    ldbl bestAvgScore = children[0]->getAvgScore();
    Node* bestChild = children[0];
    for (Node* child : children) {
        ldbl avgScore = child->getAvgScore();
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
    do {
        Node* pvLeaf = this->selectPV();
        pvLeaf->expandAndRunSim(N_TRIAL_PER_SIM);

        // update executedTime
        clock_gettime(CLOCK_REALTIME, &endTime);
        executedTime = (double)(endTime.tv_sec - startTime.tv_sec) +
                       (double)(endTime.tv_nsec - startTime.tv_nsec) * 1e-9;
    } while (executedTime < timeLimitInSec);

    // pick the ply with the highest average score
    Node* maxAvgScoreDep1Node = this->getMaxAvgScoreDep1Node();
    return maxAvgScoreDep1Node->getPly();
}