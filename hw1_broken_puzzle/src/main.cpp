#include <cstring>

#include "a_star_solver.h"
#include "board.h"
#include "brute_force_solver.h"
#include "utils.h"
using namespace std;

PuzzleSolver* getSolver(int argc, char** argv, Board* initialBoard) {
    if (argc == 2 && !strcmp(argv[1], "--brute-force")) {
        return (new BruteForceSolver(initialBoard));
    } else if (argc == 2 && !strcmp(argv[1], "--a-star")) {
        return (new AStarSolver(initialBoard));
    }
    cerr << "Usage:" << endl;
    cerr << "  [brute-force solver] ./main.out --brute-force" << endl;
    exit(1);
}

int main(int argc, char** argv) {
    int n = 0, m = 0;
    Board* initialBoard = new Board;
    cin >> n >> m >> (*initialBoard);

    PuzzleSolver* solver = getSolver(argc, argv, initialBoard);
    Board* terminalBoard = solver->solve();
    vector<pair<short, Action>> moveHistory = terminalBoard->getPrevMoves();

    cout << moveHistory.size() << endl;
    for (auto resIter = moveHistory.begin(); resIter < moveHistory.end(); resIter++) {
        cout << resIter->first << " " << resIter->second << endl;
    }

    solver->deleteAll();
    delete solver;
    return 0;
}