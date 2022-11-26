#include <array>
#include <fstream>
#include <vector>
#define N_ROW 6
#define N_COL 7
#define N_NEXT 18
using namespace std;

enum class Color { Red, Blue, Empty };

class Cube {
   public:
    Color color;
    char num;
    Cube() : color(Color::Empty), num(0) {}
    Cube(Color color, char num) : color(color), num(num) {}
    void flipColor();
};

ostream& operator<<(ostream& os, Cube cube);

class Board {
   private:
    array<array<Cube, N_COL>, N_ROW> cubes;
    Color nextTurn = Color::Red;
    void toRandNext();

   public:
    Cube& get(int r, int c) { return this->cubes[r][c]; }
    void setCube(int r, int c, Cube cube) { this->cubes[r][c] = cube; }
    void setNextTurn(Color nextTurn) { this->nextTurn = nextTurn; }
    void swap(int r1, int c1, int r2, int c2);
    void flip();
    Color getWinner() const;
    vector<Board> getNext() const;
};

ostream& operator<<(ostream& os, Board board);