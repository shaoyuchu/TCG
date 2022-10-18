#pragma once
#include <map>
#include <string>

#include "board/pattern_db_board.h"
#include "pattern_database.h"
#include "solver/a_star_solver.h"
#include "utils.h"
#define PATT_DB_INITIAL_DIR "pattern_database/"
#define DONT_CARE_CELL ((1 << (BIT_PER_CELL + 1)) - 1)

class PatternDatabaseGenerator {
   private:
    PatternDbBoard baseBoard;
    map<string, vector<int>> patterns;
    void setBaseBoard(PatternDbBoard baseBoard);
    void setPatterns(map<string, vector<int>> patterns);
    int solveSingleBoard(PatternDbBoard& board);
    void generateAllZeroOnlyInitials(int remainingCnt, int startId,
                                     PatternDbBoard& startBoard,
                                     vector<PatternDbBoard>* results);
    vector<PatternDbBoard> generateAllZeroOnlyInitials();
    void generate(PatternDbBoard& startBoard, vector<int>& remaining,
                  PatternDatabase& pattDb);

   public:
    PatternDatabaseGenerator(PatternDbBoard baseBoard, map<string, vector<int>> patterns);
    void generate();
};