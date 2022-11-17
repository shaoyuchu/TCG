#pragma once
#include <map>
#include <string>

#include "board/pattern_db_builder_board.h"
#include "pattern_database.h"
#include "solver/a_star_solver.h"
#include "utils.h"
#define PATT_DB_INITIAL_DIR "pattern_database/"
#define DONT_CARE_CELL ((1 << (BIT_PER_CELL + 1)) - 1)

class PatternDatabaseGenerator {
   private:
    PatternDbBuilderBoard baseBoard;
    map<string, vector<int>> patterns;
    void setBaseBoard(PatternDbBuilderBoard baseBoard);
    void setPatterns(map<string, vector<int>> patterns);
    int solveSingleBoard(PatternDbBuilderBoard& board);
    void generateAllZeroOnlyInitials(int remainingCnt, int startId,
                                     PatternDbBuilderBoard& startBoard,
                                     vector<PatternDbBuilderBoard>* results);
    vector<PatternDbBuilderBoard> generateAllZeroOnlyInitials();
    void generate(PatternDbBuilderBoard& startBoard, vector<int>& remaining,
                  PatternDatabase& pattDb);

   public:
    PatternDatabaseGenerator(PatternDbBuilderBoard baseBoard,
                             map<string, vector<int>> patterns);
    void generate();
};