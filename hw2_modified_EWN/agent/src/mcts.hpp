#pragma once
#include "board.hpp"
#define EXPLORATION_CONST 0.5
#define N_TRIAL_PER_SIM 100

typedef long double ldbl;

class Node {
   private:
    Board board;
    Ply ply = Ply::getPly(0, 0, 0, Direction::None);
    Node* parent = NULL;
    vector<Node*> children;
    int depth = 0;
    int simCnt = 0;
    int winCnt = 0;
    int loseCnt = 0;
    ldbl cSqrtLogSimCnt = 0;
    ldbl sqrtSimCnt = 0;
    ldbl avgScore = 0;  // (winCnt - loseCnt) / simCnt
    void updateCSqrtLogSimCnt();
    void updateSqrtSimCnt();
    void updateAvgScore();

   public:
    Node(Board board) : board(board) {}
    Node(Board board, Ply& ply, Node* parent);
    void deleteChildren();
    Ply& getPly() { return this->ply; }
    Node* getParent() const { return this->parent; }
    vector<Node*>& getChildren() { return this->children; }
    int getDepth() const { return this->depth; }
    int getSimCnt() const { return this->simCnt; }
    int getWinCnt() const { return this->winCnt; }
    int getLoseCnt() const { return this->loseCnt; }
    ldbl getAvgScore() const { return this->avgScore; }
    ldbl getUcb() const;
    void expandAndRunSim(int trialCnt);
    Node* getChildWithLargestUcb();
};

ostream& operator<<(ostream& os, Node node);

class MCTS {
   private:
    Node* root = NULL;
    Node* selectPV();

   public:
    MCTS(Board board) { this->root = new Node(board); }
    ~MCTS();
    Ply& getBestPly(double timeLimitInSec);
};

class PureMonteCarlo {
   private:
    Board board;

   public:
    PureMonteCarlo(Board board) : board(board) {}
    Ply getBestPly(double timeLimitInSec) {
        return Ply::getPly(0, 0, 0, Direction::None);
    }
};