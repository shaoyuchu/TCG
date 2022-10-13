#include "board.h"
#include "puzzle_solver.h"
#include "utils.h"
using namespace std;

int main() {
    int n = 0, m = 0;
    Board board;
    cin >> n >> m;
    cin >> board;
    cout << board << endl;

    vector<Board> nextBoards = board.getNext();
    for (auto& nextBoard : nextBoards) {
        cout << nextBoard << endl;
    }

    return 0;
}