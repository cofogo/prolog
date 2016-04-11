all: prolog

prolog: main.cpp
	mkdir -p bin
	g++ -std=c++11 main.cpp -o bin/prolog
