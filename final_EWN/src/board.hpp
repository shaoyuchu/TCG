#pragma once
#include <array>
#include <bitset>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#define BOARD_SIZE 5
#define N_CELL 25
#define N_CUBE 12
using namespace std;

enum Color { Blue, Red };

enum Direction { Vertical = 5, Horizontal = 1, Diagonal = 6 };

class Ply {
   private:
    const static map<tuple<short, Direction>, Ply> plyInstances;
    Ply(short cubeId, Direction dir) : cubeId(cubeId), dir(dir) {}

   public:
    short cubeId;
    Direction dir;
    static map<tuple<short, Direction>, Ply> initPlyInstances();
    static const Ply& getPly(short cubeId, Direction dir);
};

typedef bitset<N_CELL> Bitboard;

class Board {
   private:
    const static string cubeNames[N_CUBE];
    Color nextTurn;
    array<Bitboard, N_CUBE> bitboards;

    int getCubeByCellId(int cubeId) const;
    int getMovableCube(int dice) const;

   public:
    Board() {}
    void setCube(int cubeId, int position);
    void setNextTurn(Color color) { this->nextTurn = color; }
    Color getNextTurn(int dice) { return this->nextTurn; }
    void generateMove(vector<Ply>& result, int dice);
    string toString() const;
};