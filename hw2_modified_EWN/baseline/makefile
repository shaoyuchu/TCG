all:
	g++ -std=c++11 -D GREEDY src/baseline.cpp -o greedy
	g++ -std=c++11 -D RANDOM src/baseline.cpp -o random
	g++ -std=c++11 -D HEURISTIC src/baseline.cpp -o heuristic

random:
	g++ -std=c++11 -D RANDOM src/baseline.cpp -o random

greedy:
	g++ -std=c++11 -D GREEDY src/baseline.cpp -o greedy

heuristic:
	g++ -std=c++11 -D HEURISTIC src/baseline.cpp -o heuristic

clean:
	rm -rf greedy
	rm -rf random
	rm -rf heuristic
	rm -rf .log.*
