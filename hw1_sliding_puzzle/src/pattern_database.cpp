#include "pattern_database.h"

PatternDatabase::PatternDatabase(string fileName, Mode mode) {
    if (mode == Mode::read)
        this->file.open(fileName, fstream::in | fstream::binary);
    else if (mode == Mode::write)
        this->file.open(fileName, fstream::out | fstream::binary);
}

PatternDatabase::~PatternDatabase() { file.close(); }

void PatternDatabase::write(Board& board, int moveCnt) {
    struct Entry entry;
    entry.BoardBs = board.toBitset();
    entry.moveCnt = moveCnt;
    this->file.write((char*)&entry, sizeof(entry));
}

unordered_map<bitset<BITSET_LEN>, int> PatternDatabase::read() {
    struct Entry entry;
    unordered_map<bitset<BITSET_LEN>, int> result;
    while (this->file.read((char*)&entry, sizeof(entry))) {
        result.insert(make_pair(entry.BoardBs, entry.moveCnt));
    }
    return result;
}
