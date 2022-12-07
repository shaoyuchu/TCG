#pragma once
#include <mutex>

#include "board.hpp"
#define OPENING_EXPLORATION_CONST 3.0
#define MIDDLE_EXPLORATION_CONST 2.8284
#define ENDING_EXPLORATION_CONST 2.0
#define TOTAL_DISTANCE_THRES1 105
#define TOTAL_DISTANCE_THRES2 80
#define RAVE_RATIO_DECAY_RATE 0.005
#define N_THREAD 4
#define N_TRIAL_PER_SIM 100

typedef long double ldbl;
typedef long long ll;

class Node {
    friend class MCTS;
    friend ostream& operator<<(ostream& os, Node node);

   private:
    static ldbl explorationConst;
    Board board;
    Ply ply = Ply::getPly(0, 0, 0, Direction::None);
    Node* parent = NULL;
    vector<Node*> children;
    int depth = 0;
    ll simCnt = 0;
    ll winCnt = 0;
    ll loseCnt = 0;
    ldbl cSqrtLogSimCnt = 0;
    ldbl sqrtSimCnt = 0;
    ldbl avgScore = 0;
    ll amafSimCnt = 0;
    ll amafWinCnt = 0;
    ll amafLoseCnt = 0;
    ldbl amafAvgScore = 0;
    mutex simResLock;
    void updateCompositeStat();
    void updateAmafCompositeStat();
    void expandChildren();
    void runSimulation(int trialCnt);
    void runSimOnChildren(int trialCnt);
    void amafUpdate();
    void addActualSimRes(int simCnt, int winCnt, int loseCnt);
    void addVirtualSimRes(int simCnt, int winCnt, int loseCnt);

   public:
    Node(Board board) : board(board) {}
    Node(Board board, Ply& ply, Node* parent);
    void deleteChildren();
    ldbl getUcb() const;
    void expandAndRunSim(int trialCnt);
    Node* getChildWithLargestUcb() const;
};

ostream& operator<<(ostream& os, Node node);

class MCTS {
   private:
    int nodeCnt = 0;
    int leafCnt = 0;
    int totalLeafDepth = 0;
    int maxDepth = 0;
    Node* root = NULL;
    Node* selectPV() const;
    Node* getMaxAvgScoreDep1Node() const;

   public:
    MCTS(Board board);
    ~MCTS();
    Ply& getBestPly(double timeLimitInSec);
};

class PureMonteCarlo {
   private:
    Board board;
    ll simCnt = 0;

   public:
    PureMonteCarlo(Board board) : board(board) {}
    ~PureMonteCarlo();
    Ply& getBestPly(double timeLimitInSec);
};