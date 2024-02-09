CC=g++
CFLAGS=-Wall -O3 -std=c++17

all: exh greedy mh parser checker

exh: exh.cc
	$(CC) $(CFLAGS) -o exh.out algorithms/exh.cc

greedy: greedy.cc
	$(CC) $(CFLAGS) -o greedy.out algorithms/greedy.cc

mh: mh.cc
	$(CC) $(CFLAGS) -o mh.out algorithms/mh.cc


parser: parser.cc
	$(CC) $(CFLAGS) -o parser.out parser.cc

checker: checker.cc
	$(CC) $(CFLAGS) -o checker.out checker.cc