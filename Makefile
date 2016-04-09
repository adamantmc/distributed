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
	mpicc -fopenmp -o examine_reduce.out examine_reduce.c 
	mpirun ./examine_reduce.out -1 -1 data.txt -1 -1 	

examineomp.o: examineomp.c
	$(CC) $(CFF) examineomp.c
	
clean: 
	rm -rf *.o
	rm -rf *.out
	rm -rf *.txt 




