#include "agent.hpp"

#include <stdlib.h>
#include <time.h>

#include <iostream>

#include "solver.hpp"
using namespace std;

const string Agent::cellNames[N_CELL] = {
    "A1", "B1", "C1", "D1", "E1", "A2", "B2", "C2", "D2", "E2", "A3", "B3", "C3",
    "D3", "E3", "A4", "B4", "C4", "D4", "E4", "A5", "B5", "C5", "D5", "E5"};

void Agent::shuffle(array<int, 6>& order) {
    for (int s = 0; s < 20; s++) {
        int i = rand() % 6;
        int j = rand() % 6;
        if (i != j) {
            int tmp = order[i];
            order[i] = order[j];
            order[j] = tmp;
        }
    }
}

void Agent::setCubes(const char* data[], Board& board) {
    for (int i = 0; i < N_CUBE; i++) {
        if (data[i][0] == '0') continue;
        int position = (data[i][1] - '1') * 5 + (data[i][0] - 'A');
        board.setCube(i, position);
    }
}

Agent::Agent() { srand(time(NULL)); }

void Agent::name(const char* data[], char* response) { strcpy(response, "MyAI"); }

void Agent::version(const char* data[], char* response) { strcpy(response, "1.0.0"); }

void Agent::timeSetting(const char* data[], char* response) {
    this->timeLimit = stoi(data[1]);
    strcpy(response, "1");
}

void Agent::boardSetting(const char* data[], char* response) {
    // setup board size, number of cubes, number of players
    strcpy(response, "1");
}

void Agent::init(const char* data[], char* response) {
    array<int, 6> order = {0, 1, 2, 3, 4, 5};
    this->shuffle(order);

    string blueInitPos[6] = {"E3", "D4", "E4", "C5", "D5", "E5"};
    string redInitPos[6] = {"A1", "B1", "C1", "A2", "B2", "A3"};
    string* initPos = ((!strcmp(data[1], "R")) ? redInitPos : blueInitPos);
    string result = initPos[order[0]];
    for (int i = 1; i < 6; i++) {
        result += " " + initPos[order[i]];
    }
    strcpy(response, result.c_str());
}

void Agent::get(const char* data[], char* response) {
    int dice = stoi(data[2]);
    cerr << "-------------------------" << endl;
    cerr << "Dice: " << dice << endl;

    Board board;
    Color nextTurn = (!strcmp(data[1], "R")) ? Color::Red : Color::Blue;
    board.setNextTurn(nextTurn);
    this->setCubes(&data[3], board);
    cerr << board.toString();

    Solver solver(board);
    Ply bestPly = solver.getBestPly(dice);
    cerr << "Final Decision: " << bestPly.toString() << endl;

    int src = board.getCellByCubeId(bestPly.getCubeId());
    int dest = (nextTurn == Color::Red ? src + bestPly.getDir() : src - bestPly.getDir());
    sprintf(response, "%s %s", Agent::cellNames[src].c_str(),
            Agent::cellNames[dest].c_str());
}
