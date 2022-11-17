CPLUSPLUS = g++ -std=c++11 -O2 -Wall
DEL = rm -f
F_SIX = -D SIX
F_SEVEN = -D SEVEN
F_SHUFFLE = -D INIT_RANDOM_SHUFFLE
F_LINUX = -D LINUX

game:
	$(CPLUSPLUS) $(F_SHUFFLE) $(F_SIX) $(F_LINUX) src/game.cpp -o game

clean:
	$(DEL) game
	$(DEL) .log.*