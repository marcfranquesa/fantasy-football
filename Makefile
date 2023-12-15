CC=g++
CFLAGS=-Wall -O3 -std=c++17

all: exh greedy parser checker
test: test_exh test_greedy test_mh

exh: exh.cc
	$(CC) $(CFLAGS) -o exh.out exh.cc
test_exh: test_exh.cc
	$(CC) $(CFLAGS) -o exh.out test_exh.cc

greedy: greedy.cc
	$(CC) $(CFLAGS) -o greedy.out greedy.cc
test_greedy: test_greedy.cc
	$(CC) $(CFLAGS) -o greedy.out test_greedy.cc

mh: mh.cc
	$(CC) $(CFLAGS) -o mh.out mh.cc
test_mh: test_mh.cc
	$(CC) $(CFLAGS) -o mh.out test_mh.cc


parser: parser.cc
	$(CC) $(CFLAGS) -o parser.out parser.cc

checker: checker.cc
	$(CC) $(CFLAGS) -o checker.out checker.cc