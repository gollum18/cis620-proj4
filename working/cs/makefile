CC=gcc
CFLAGS=-g -Wall
IFLAGS=-I.

.PHONY: all clean depend submit

all: client server

client: client.o
	gcc -o client client.o

server: server.o
	gcc -o server server.o -lm

clean:
	rm *.o client server

submit:
	turnin -c cis620s -p proj4 *.c makefile report.pdf

depend:
	makedepend $(IFLAGS) *.c
