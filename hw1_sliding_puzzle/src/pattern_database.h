#pragma once
#include <fstream>
#include <string>
#include <unordered_map>

#include "board/pattern_db_builder_board.h"

enum Mode { read, write };

class PatternDatabase {
    struct Entry {
        bitset<BITSET_LEN> BoardBs;
        int moveCnt;
    };

   private:
    fstream file;

   public:
    PatternDatabase(string fileName, Mode mode);
    ~PatternDatabase();
    void write(Board& board, int moveCnt);
    unordered_map<bitset<BITSET_LEN>, int> read();
};