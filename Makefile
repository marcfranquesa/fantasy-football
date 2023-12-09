CC=g++
CFLAGS=-Wall -O3 -std=c++17

all: exh parser checker
test: test_exh

exh: exh.cc
	$(CC) $(CFLAGS) -o exh.out exh.cc

test_exh: test_exh.cc
	$(CC) $(CFLAGS) -o exh.out test_exh.cc




parser: parser.cc
	$(CC) $(CFLAGS) -o parser.out parser.cc

checker: checker.cc
	$(CC) $(CFLAGS) -o checker.out checker.cc