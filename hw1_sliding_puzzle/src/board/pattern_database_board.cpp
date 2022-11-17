#include "pattern_database_board.h"

int PatternDatabaseBoard::getPatternHeuristicCost() {
    int total = 0;
    for (auto& patternIter : *(this->patterns)) {
        // erase numbers that are not in the pattern
        Board board(*this);
        const bitset<M* N>& pattern = patternIter.first;
        const unordered_map<bitset<BITSET_LEN>, int>& database = patternIter.second;
        for (int i = 0; i < M * N; i++) {
            int number = board.get(i);
            if (number != 0 && number != -1 && !pattern[number]) board.set(i, -1);
        }

        // lookup database
        auto foundEntry = database.find(board.toBitset());
        if (foundEntry != database.end()) total += foundEntry->second;
    }
    return total;
}

Board* PatternDatabaseBoard::duplicate() {
    Board* newBoard = new PatternDatabaseBoard(*this);
    newBoard->clearCache();
    return newBoard;
}

void PatternDatabaseBoard::setPatterns(
    unordered_map<bitset<M * N>, unordered_map<bitset<BITSET_LEN>, int>>* patterns) {
    this->patterns = patterns;
}

int PatternDatabaseBoard::getCost() {
    int manhattanCost = this->AStarBoard::getCost();
    int patternCost = this->prevMoves.size() + this->getPatternHeuristicCost();
    return max(manhattanCost, patternCost);
}