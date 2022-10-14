#include "board.h"
#include "brute_force_solver.h"
#include "utils.h"
using namespace std;

int main() {
    int n = 0, m = 0;
    Board initialBoard;
    cin >> n >> m >> initialBoard;

    BruteForceSolver solver(initialBoard);
    vector<pair<short, Action>> result = solver.solve();

    cout << result.size() << endl;
    for (auto resIter = result.begin(); resIter < result.end(); resIter++) {
        cout << resIter->first << " " << resIter->second << endl;
    }

    return 0;
}