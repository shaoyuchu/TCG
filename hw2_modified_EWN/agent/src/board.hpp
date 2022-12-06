#pragma once
#include <array>
#include <fstream>
#include <map>
#include <tuple>
#include <vector>
#define N_ROW 6
#define N_COL 7
#define N_NEXT 18
using namespace std;

// Color
enum class Color { Red, Blue, Empty };

ostream& operator<<(ostream& os, Color color);

// Cube
class Cube {
   public:
    Color color;
    char num;
    Cube() : color(Color::Empty), num(0) {}
    Cube(Color color, char num) : color(color), num(num) {}
    void flipColor();
};

ostream& operator<<(ostream& os, Cube cube);

// Direction
enum class Direction { Vertical, Horizontal, Diagonal, None };

// Ply
class Ply {
   private:
    const static map<tuple<int, int, char, Direction>, Ply> plyInstances;
    Ply(int row, int col, char num, Direction dir)
        : row(row), col(col), num(num), dir(dir) {}

   public:
    int row;
    int col;
    char num;
    Direction dir;
    static map<tuple<int, int, char, Direction>, Ply> initPlyInstances();
    static const Ply& getPly(int r, int c, char num, Direction dir);
};

ostream& operator<<(ostream& os, Ply ply);

// Board
class Board {
   private:
    array<array<Cube, N_COL>, N_ROW> cubes;
    vector<Ply> validPlys;
    Color nextTurn = Color::Red;
    void swap(int r1, int c1, int r2, int c2);

   public:
    Board() { this->validPlys.reserve(18); }
    Cube& get(int r, int c) { return this->cubes[r][c]; }
    Color getNextTurn() { return this->nextTurn; }
    void setCube(int r, int c, Cube cube) { this->cubes[r][c] = cube; }
    void flipNextTurn();
    void flip();
    bool isCompleted() const;
    Color getWinner() const;
    vector<Ply>* getAllValidPly();
    void applyPly(const Ply& ply);
    void applyPly(char num, Direction dir);
    Color getRandomPlayWinner() const;
    int getTotalDistanceToCorner() const;
};

ostream& operator<<(ostream& os, Board board);