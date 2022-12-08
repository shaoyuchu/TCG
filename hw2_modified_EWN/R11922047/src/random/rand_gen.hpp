#include <stdlib.h>

#include "pcg_random.hpp"
#define SEED 0

class CppRandGen {
   private:
    static bool hasInit;

   public:
    static unsigned int getRandNum(unsigned int upperBound);
};

class PcgRandGen {
   private:
    static pcg32 rng;

   public:
    static unsigned int getRandNum(unsigned int upperBound);
};

bool CppRandGen::hasInit = false;

unsigned int CppRandGen::getRandNum(unsigned int upperBound) {
    if (CppRandGen::hasInit == false) {
        srand(SEED);
        CppRandGen::hasInit = true;
    }
    return (rand() % upperBound);
}

pcg32 PcgRandGen::rng = pcg32(SEED);

unsigned int PcgRandGen::getRandNum(unsigned int upperBound) {
    return PcgRandGen::rng(upperBound);
}
