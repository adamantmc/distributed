CC=gcc
CFLAGS=-c -Wall

all: generate examine

generate: generator.o
	$(CC) generator.o -o generator.out	
	./generator.out

generator.o: generator.c
	$(CC) $(CFLAGS) generator.c

examine: aaa.o
	$(CC) aaa.o -o aaa.out
	./aaa.out	

aaa.o: aaa.c
	$(CC) $(CFLAGS) aaa.c
	
clean: 
	rm -rf *o hello
	rm -rf *out hello
	rm -rf *txt hello




