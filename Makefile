CC=gcc
CFLAGS=-c -Wall
CFF=  -c -fopenmp
all: generate examine

generate: generator.o
	$(CC) generator.o -o generator.out	
	./generator.out data.txt 15000000

generator.o: generator.c
	$(CC) $(CFLAGS) generator.c

examine: 
	#$(CC) examineomp.o -o examineomp.out
	gcc -fopenmp examineomp.c -o examineomp.out
	./examineomp.out -1 -1 data.txt 20 1 	

examineomp.o: examineomp.c
	$(CC) $(CFF) examineomp.c
	
clean: 
	rm -rf *.o
	rm -rf *.out
	rm -rf *.txt 




