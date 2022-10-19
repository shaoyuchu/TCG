#include "pattern_database_solver.h"

#include <fstream>
#include <sstream>

PatternDatabaseSolver::PatternDatabaseSolver(string configPath) {
    ifstream file(configPath);
    string buffer, fileName, pattern;
    for (int i = 0; i < M; i++) getline(file, buffer);  // ignore the base board
    while (getline(file, fileName)) {
        getline(file, pattern);

        // generate pattern bitseet
        bitset<M * N> patternBitset;
        stringstream ss(pattern);
        vector<int> numbers;
        int num;
        while (ss >> num) patternBitset[num] = 1;

        // read pattern database
        PatternDatabase pattDb(fileName, Mode::read);
        unordered_map<bitset<BITSET_LEN>, int> database = pattDb.read();
        this->patterns.insert(make_pair(patternBitset, database));
    }
    file.close();
}

void PatternDatabaseSolver::init(Board* initialBoard) {
    if (dynamic_cast<PatternDatabaseBoard*>(initialBoard)) {
        PatternDatabaseBoard* castedBoard =
            dynamic_cast<PatternDatabaseBoard*>(initialBoard);
        castedBoard->setPatterns(&(this->patterns));
    } else {
        cerr << "Error: PatternDatabaseSolver should use PatternDatabaseBoard." << endl;
        exit(1);
    }
    this->AStarSolver::init(initialBoard);
}