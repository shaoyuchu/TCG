#include <cstring>

#include "board.h"
#include "brute_force_solver.h"
#include "utils.h"
using namespace std;

PuzzleSolver* getSolver(int argc, char** argv, Board& initialBoard) {
    if (argc == 2 && !strcmp(argv[1], "--brute-force")) {
        return (new BruteForceSolver(initialBoard));
    }
    cerr << "Usage:" << endl;
    cerr << "  [brute-force solver] ./main.out --brute-force" << endl;
    exit(1);
}

int main(int argc, char** argv) {
    int n = 0, m = 0;
    Board initialBoard;
    cin >> n >> m >> initialBoard;

    PuzzleSolver* solver = getSolver(argc, argv, initialBoard);
    vector<pair<short, Action>> result = solver->solve();

    cout << result.size() << endl;
    for (auto resIter = result.begin(); resIter < result.end(); resIter++) {
        cout << resIter->first << " " << resIter->second << endl;
    }

    delete solver;
    return 0;
}