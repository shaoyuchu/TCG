#include "pattern_database_generator.h"

#include <sstream>
#include <string>

void PatternDatabaseGenerator::setPatterns(map<string, vector<int>> patterns) {
    bool exists[M * N] = {false};
    for (auto& pattern : patterns) {
        for (int number : pattern.second) {
            if (number <= 0 || number >= (M * N)) {
                cerr << "Error: Invalid patterns." << endl;
                exit(1);
            }
            if (exists[number]) {
                cerr << "Error: Duplicated numbers in the patterns." << endl;
                exit(1);
            }
            exists[number] = true;
        }
    }
    this->patterns = patterns;
}

void PatternDatabaseGenerator::setBaseBoard(PatternDbBuilderBoard baseBoard) {
    // set all movable cells to the largest number to deal with the zeros placed
    // afterwards
    for (int i = 0; i < M * N; i++) {
        if (baseBoard.get(i) == 0) baseBoard.set(i, DONT_CARE_CELL);
    }
    this->baseBoard = baseBoard;
}

PatternDatabaseGenerator::PatternDatabaseGenerator(PatternDbBuilderBoard baseBoard,
                                                   map<string, vector<int>> patterns) {
    this->setBaseBoard(baseBoard);
    this->setPatterns(patterns);
}

int PatternDatabaseGenerator::solveSingleBoard(PatternDbBuilderBoard& board) {
    AStarSolver solver;
    Board* boardToSolve = new PatternDbBuilderBoard(board);
    solver.init(boardToSolve);
    Board terminalBoard = solver.solve();
    int moveCnt = terminalBoard.getPrevMoves().size();

    solver.deleteAll();
    return moveCnt;
}

void PatternDatabaseGenerator::generateAllZeroOnlyInitials(
    int remainingCnt, int startId, PatternDbBuilderBoard& startBoard,
    vector<PatternDbBuilderBoard>* results) {
    if (remainingCnt > 0 && startId >= M * N) return;
    if (remainingCnt == 0) {
        results->push_back(startBoard);
        return;
    }
    for (int i = startId; i < M * N; i++) {
        if (startBoard.get(i) != DONT_CARE_CELL) continue;
        startBoard.set(i, 0);  // set
        this->generateAllZeroOnlyInitials(remainingCnt - 1, i + 1, startBoard, results);
        startBoard.set(i, DONT_CARE_CELL);  // reset
    }
}

vector<PatternDbBuilderBoard> PatternDatabaseGenerator::generateAllZeroOnlyInitials() {
    vector<PatternDbBuilderBoard> allZeroOnlyInitials;
    PatternDbBuilderBoard startBoard(this->baseBoard);
    this->generateAllZeroOnlyInitials(N_EMPTY, 0, startBoard, &allZeroOnlyInitials);
    return allZeroOnlyInitials;
}

void PatternDatabaseGenerator::generate(PatternDbBuilderBoard& startBoard,
                                        vector<int>& remaining, PatternDatabase& pattDb) {
    // all numbers are filled in, write result to database
    if (remaining.size() == 0) {
        int moveCnt = this->solveSingleBoard(startBoard);
        pattDb.write(startBoard, moveCnt);
        return;
    }

    // fill in the last number
    int numToFillIn = remaining.back();
    remaining.pop_back();
    for (int pos = 0; pos < M * N; pos++) {
        if (startBoard.get(pos) == DONT_CARE_CELL) {
            startBoard.set(pos, numToFillIn);  // set number
            this->generate(startBoard, remaining, pattDb);
            startBoard.set(pos, DONT_CARE_CELL);  // reset number
        }
    }
    remaining.push_back(numToFillIn);
}

void PatternDatabaseGenerator::generate() {
    for (auto& pattern : this->patterns) {
        string patternFileName = pattern.first;
        PatternDatabase pattDb(patternFileName, Mode::write);
        vector<PatternDbBuilderBoard> allZeroOnlyInitials =
            this->generateAllZeroOnlyInitials();
        for (PatternDbBuilderBoard& startBoard : allZeroOnlyInitials) {
            startBoard.setPattern(pattern.second);
            this->generate(startBoard, pattern.second, pattDb);
        }
        cout << patternFileName << " generated" << endl;
    }
}

void readPatterns(PatternDbBuilderBoard* baseBoard, map<string, vector<int>>* patterns) {
    cin >> (Board*)baseBoard;
    cin.ignore();  // ignore newline

    string fileName;
    string pattern;
    while (getline(cin, fileName)) {
        getline(cin, pattern);
        stringstream ss(pattern);
        vector<int> numbers;
        int num;
        while (ss >> num) {
            numbers.push_back(num);
        }
        patterns->insert(pair<string, vector<int>>(fileName, numbers));
    }
}

int main() {
    PatternDbBuilderBoard pattDbBoard;
    map<string, vector<int>> patterns;
    readPatterns(&pattDbBoard, &patterns);

    PatternDatabaseGenerator pattDbGen(pattDbBoard, patterns);
    pattDbGen.generate();

    return 0;
}