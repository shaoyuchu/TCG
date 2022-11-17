#pragma once
#include <queue>
#include <unordered_set>
#include <vector>

#include "../board/board.h"
#include "puzzle_solver.h"
using namespace std;

class CostComparison {
   public:
    bool operator()(Board* left, Board* right) {
        return left->getCost() > right->getCost();
    }
};

class PQWithRemove : public priority_queue<Board*, vector<Board*>, CostComparison> {
   private:
    unordered_set<bitset<BITSET_LEN>> items;

   public:
    void replaceIfSmallerCost(Board* replaceBy);
    void push(Board* board);
    void pop();
    bool exist(Board* board);
    void deleteAll();
};

class AStarSolver : public PuzzleSolver {
   private:
    PQWithRemove priorityQueue;

   public:
    void init(Board* initialBoard);
    Board solve();
    void deleteAll();
};