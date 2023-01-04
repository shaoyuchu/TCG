#pragma once
#include <array>
#include <bitset>
#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#define BOARD_SIZE 5
#define N_CELL 25
#define N_CUBE 12
using namespace std;

enum Color { Blue = 0, Red = 1, None };

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
    size_t hash;
    const static map<tuple<Color, Direction>, Bitboard> moveMasks;
    const static unordered_map<Bitboard, Bitboard> blueDests;
    const static unordered_map<Bitboard, Bitboard> redDests;
    const static array<array<size_t, N_CELL>, N_CUBE> cubePosHashKey;
    const static array<size_t, 2> colorHashKey;
    const static array<size_t, 6> diceHashKey;

    static map<tuple<Color, Direction>, Bitboard> initMoveMasks();
    static unordered_map<Bitboard, Bitboard> initBlueDests();
    static unordered_map<Bitboard, Bitboard> initRedDests();
    static array<array<size_t, N_CELL>, N_CUBE> initCubePosHashKey();
    static array<size_t, 2> initColorHashKey();
    static array<size_t, 6> initDiceHashKey();
    static const Bitboard& getMoveMask(Color nextTurn, Direction dir);
    int getCubeByCellId(int cellId) const;
    pair<int, int> getMovableCubes(int dice) const;

   public:
    const static string cubeNames[N_CUBE];
    Board() : hash(0) {}
    void setCube(int cubeId, int position);
    void setNextTurn(Color color);
    Color getNextTurn() const { return this->nextTurn; }
    int getCellByCubeId(int cubeId) const;
    Color getWinner() const;
    void generateMoves(vector<Ply>& result, int dice) const;
    bitset<12> cubeExist() const;
    void applyPly(Ply& ply);
    void getCapturableCubes(vector<int>& result, int cubeId) const;
    size_t getHash(int dice) const;
    string toString() const;
};