#include <fstream>

#include "a_star_solver.h"
#include "board/pattern_db_board.h"
#define PATT_DB_INITIAL_DIR "pattern_database/"
#define DONT_CARE_CELL ((1 << (BIT_PER_CELL + 1)) - 1)

class PatternDatabaseGenerator {
   private:
    Board baseBoard;
    vector<vector<int>> patterns;
    void setBaseBoard(Board baseBoard);
    void setPatterns(vector<vector<int>> patterns);
    void generateAllZeroOnlyInitials(int remainingCnt, int startId, Board& startBoard,
                                     vector<Board>* results);
    vector<Board> generateAllZeroOnlyInitials();
    void generateAllInitials(Board& startBoard, vector<int>& remaining,
                             ofstream& outfile);
    void generateAllInitials();

   public:
    PatternDatabaseGenerator(Board baseBoard, vector<vector<int>> patterns);
    void generate() { generateAllInitials(); }
};