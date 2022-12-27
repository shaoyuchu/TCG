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
    const static map<tuple<int, Direction>, Ply> plyInstances;

    static map<tuple<int, Direction>, Ply> initPlyInstances();
    Ply(int cubeId, Direction dir) : cubeId(cubeId), dir(dir) {}

   public:
    int cubeId;
    Direction dir;

    static const Ply& getPly(int cubeId, Direction dir);
    int getCubeId() const { return this->cubeId; }
    Direction getDir() const { return this->dir; }
    string toString() const;
};

typedef bitset<N_CELL> Bitboard;

class Board {
   private:
    Color nextTurn;
    array<Bitboard, N_CUBE> bitboards;
    const static map<tuple<Color, Direction>, Bitboard> moveMasks;

    static map<tuple<Color, Direction>, Bitboard> initMoveMasks();
    static const Bitboard& getMoveMask(Color nextTurn, Direction dir);
    int getCubeByCellId(int cellId) const;
    pair<int, int> getMovableCubes(int dice) const;

   public:
    const static string cubeNames[N_CUBE];
    Board() {}
    void setCube(int cubeId, int position);
    void setNextTurn(Color color) { this->nextTurn = color; }
    Color getNextTurn() const { return this->nextTurn; }
    int getCellByCubeId(int cubeId) const;
    void generateMoves(vector<Ply>& result, int dice) const;
    bitset<12> cubeExist() const;
    void applyPly(Ply& ply);
    string toString() const;
};