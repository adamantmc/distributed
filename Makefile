CC=gcc
CFLAGS=-c -Wall

all: generate examine

generate: generator.o
	$(CC) generator.o -o generator.out	
	./generator.out

generator.o: generator.c
	$(CC) $(CFLAGS) generator.c

examine: examine.o
	$(CC) examine.o -o examine.out
	./examine.out	

examine.o: examine.c
	$(CC) $(CFLAGS) examine.c
	
clean: 
	rm -rf *.o
	rm -rf *.out
	rm -rf *.txt 




