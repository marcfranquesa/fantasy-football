CC=g++
CFLAGS=-Wall -O3 -std=c++17

all: exh greedy mh parser checker

exh: algorithms/exh.cc
	$(CC) $(CFLAGS) -o exh.out algorithms/exh.cc

greedy: algorithms/greedy.cc
	$(CC) $(CFLAGS) -o greedy.out algorithms/greedy.cc

mh: algorithms/mh.cc
	$(CC) $(CFLAGS) -o mh.out algorithms/mh.cc


parser: scripts/parser.cc
	$(CC) $(CFLAGS) -o parser.out scripts/parser.cc

checker: scripts/checker.cc
	$(CC) $(CFLAGS) -o checker.out scripts/checker.cc