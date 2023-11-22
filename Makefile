all:
	g++ --std=c++17 -pedantic -Wall  -o switcher switcher.cpp -O0 -ggdb -lX11 -lXi -lXtst

