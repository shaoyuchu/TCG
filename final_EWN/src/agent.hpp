#pragma once
#include <array>
#include <cstring>

#include "board.hpp"
using namespace std;

class Agent {
   private:
    double timeLimitInSec;
    double timeUsedInSec;
    const static string cellNames[N_CELL];
    void shuffle(array<int, 6>& order);
    void setCubes(const char* data[], Board& board);

   public:
    Agent();
    void name(const char* data[], char* response);
    void version(const char* data[], char* response);
    void timeSetting(const char* data[], char* response);
    void boardSetting(const char* data[], char* response);
    void init(const char* data[], char* response);
    void get(const char* data[], char* response);
    void exit(const char* data[], char* response);
};