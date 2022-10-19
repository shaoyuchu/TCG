#include "pattern_database_solver.h"

#include <fstream>
#include <sstream>

#include "../pattern_database.h"

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
    this->AStarSolver::init(initialBoard);
    // TODO: set pattern database ptr after adding new board type
}