CC=g++
CFLAGS=-Wall -O3 -std=c++17

all: exh parser checker

exh: exh.cc
	$(CC) $(CFLAGS) -o exh.out exh.cc

test: test.cc
	$(CC) $(CFLAGS) -o exh.out test.cc

parser: parser.cc
	$(CC) $(CFLAGS) -o parser.out parser.cc

checker: checker.cc
	$(CC) $(CFLAGS) -o checker.out checker.cc