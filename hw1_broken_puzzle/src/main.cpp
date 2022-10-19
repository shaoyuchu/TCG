#include <cstring>

#include "board/a_star_board.h"
#include "solver/a_star_solver.h"
#include "solver/brute_force_solver.h"
#include "solver/pattern_database_solver.h"
#include "utils.h"
using namespace std;

PuzzleSolver* getSolver(int argc, char** argv) {
    if (argc == 2 && !strcmp(argv[1], "--brute-force")) {
        return (new BruteForceSolver());
    } else if (argc == 2 && !strcmp(argv[1], "--a-star")) {
        return (new AStarSolver());
    } else if (argc == 3 && !strcmp(argv[1], "--pattern-database")) {
        return (new PatternDatabaseSolver(argv[2]));
    }
    cerr << "Usage:" << endl;
    cerr << "  [brute-force solver] ./main.out --brute-force" << endl;
    cerr << "  [A-star solver] ./main.out --a-star" << endl;
    cerr << "  [pattern-database solver] ./main.out --pattern-database "
            "<database-config-path>"
         << endl;
    exit(1);
}

Board* getInitialBoard(PuzzleSolver* solver) {
    Board* initialBoard;
    if (typeid(*solver) == typeid(BruteForceSolver)) {
        initialBoard = new Board();
    } else if (typeid(*solver) == typeid(AStarSolver)) {
        initialBoard = new AStarBoard();
    } else if (typeid(*solver) == typeid(PatternDatabaseSolver)) {
        initialBoard = new AStarBoard();
    } else {
        cerr << "Error: Invalid solver.\n";
        exit(1);
    }
    cin >> initialBoard;
    return initialBoard;
}

int main(int argc, char** argv) {
    int n = 0, m = 0;
    cin >> n >> m;

    PuzzleSolver* solver = getSolver(argc, argv);
    Board* initialBoard = getInitialBoard(solver);
    solver->init(initialBoard);
    Board terminalBoard = solver->solve();
    vector<pair<short, Action>> moveHistory = terminalBoard.getPrevMoves();

    cout << moveHistory.size() << endl;
    for (auto resIter = moveHistory.begin(); resIter < moveHistory.end(); resIter++) {
        cout << resIter->first << " " << resIter->second << endl;
    }

    solver->deleteAll();
    delete solver;
    return 0;
}