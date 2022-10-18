#include "pattern_database_generator.h"

#include <sstream>
#include <string>

#include "utils.h"

void PatternDatabaseGenerator::setPatterns(vector<vector<int>> patterns) {
    bool exists[M * N] = {false};
    for (vector<int> pattern : patterns) {
        for (int number : pattern) {
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

void PatternDatabaseGenerator::setBaseBoard(Board baseBoard) {
    // set all movable cells to the largest number to deal with the zeros placed
    // afterwards
    for (int i = 0; i < M * N; i++) {
        if (baseBoard.operator()(i) == 0) baseBoard.operator()(i) = DONT_CARE_CELL;
    }
    this->baseBoard = baseBoard;
}

PatternDatabaseGenerator::PatternDatabaseGenerator(Board baseBoard,
                                                   vector<vector<int>> patterns) {
    this->setBaseBoard(baseBoard);
    this->setPatterns(patterns);
}

void PatternDatabaseGenerator::generateAllZeroOnlyInitials(int remainingCnt, int startId,
                                                           Board& startBoard,
                                                           vector<Board>* results) {
    if (remainingCnt > 0 && startId >= M * N) return;
    if (remainingCnt == 0) {
        results->push_back(startBoard);
        return;
    }
    for (int i = startId; i < M * N; i++) {
        if (startBoard.operator()(i) != DONT_CARE_CELL) continue;
        startBoard.operator()(i) = 0;  // set
        this->generateAllZeroOnlyInitials(remainingCnt - 1, i + 1, startBoard, results);
        startBoard.operator()(i) = DONT_CARE_CELL;  // reset
    }
}

vector<Board> PatternDatabaseGenerator::generateAllZeroOnlyInitials() {
    vector<Board> allZeroOnlyInitials;
    Board startBoard(this->baseBoard);
    this->generateAllZeroOnlyInitials(N_EMPTY, 0, startBoard, &allZeroOnlyInitials);
    return allZeroOnlyInitials;
}

void PatternDatabaseGenerator::generate(Board& startBoard, vector<int>& remaining,
                                        ofstream& outfile) {
    // all numbers are filled in, write result to file
    if (remaining.size() == 0) {
        outfile << (&startBoard) << endl;
        return;
    }

    // fill in the last number
    int numToFillIn = remaining.back();
    remaining.pop_back();
    for (int pos = 0; pos < M * N; pos++) {
        if (startBoard.operator()(pos) == DONT_CARE_CELL) {
            startBoard.operator()(pos) = numToFillIn;  // set number
            this->generate(startBoard, remaining, outfile);
            startBoard.operator()(pos) = DONT_CARE_CELL;  // reset number
        }
    }
    remaining.push_back(numToFillIn);
}

void PatternDatabaseGenerator::generate() {
    for (vector<int> pattern : this->patterns) {
        ofstream outfile;
        outfile.open(PATT_DB_INITIAL_DIR + pattern2Str(pattern));
        vector<Board> allZeroOnlyInitials = this->generateAllZeroOnlyInitials();
        for (Board startBoard : allZeroOnlyInitials) {
            this->generate(startBoard, pattern, outfile);
        }
        outfile.close();
        cout << PATT_DB_INITIAL_DIR + pattern2Str(pattern) << " generated" << endl;
        ;
    }
}

void readPatterns(Board* baseBoard, vector<vector<int>>* patterns) {
    cin >> baseBoard;
    cin.ignore();  // ignore newline

    string line;
    while (getline(cin, line)) {
        stringstream ss(line);
        vector<int> numbers;
        int num;
        while (ss >> num) {
            numbers.push_back(num);
        }
        patterns->push_back(numbers);
    }
}

int main() {
    Board pattDbBoard;
    vector<vector<int>> patterns;
    readPatterns(&pattDbBoard, &patterns);

    PatternDatabaseGenerator pattDbGen(pattDbBoard, patterns);
    pattDbGen.generate();

    return 0;
}