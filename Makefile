CC=g++
CFLAGS=-Wall -O3 -std=c++17

exh: exh.cc
	$(CC) $(CFLAGS) -o exh.out exh.cc

parser: parser.cc
	$(CC) $(CFLAGS) -o parser.out parser.cc