#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define STR_LEN 31
#define BUF_SIZE 5000000
#define TOK_LEN 9

void printTime(struct timespec start, struct timespec end);

int main(int argc, char *argv[])
{
    int rank, size, i;
	FILE *data = fopen("file1.txt","r");
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for(i=0; i<size; i++) {
        if(rank == i) {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            char **buffer = malloc(sizeof(char *) * BUF_SIZE);
            int j;
            for(j=0; j<BUF_SIZE; j++) buffer[j] = malloc(sizeof(char) * STR_LEN);
			
			fseek(data,5*i,SEEK_SET);

            for(j=0; j<BUF_SIZE; j++) free(buffer[j]);
            free(buffer);
            clock_gettime(CLOCK_MONOTONIC, &end);
            printTime(start,end);
        }
    }
	for(i=0; i<size; i++) {
        if(rank == i) {
            printf("%ld \n",ftell(data));
        }
    }
    MPI_Finalize();
    return 0;
}

long read(FILE *data, char **buffer, long limit, long totalReadLines, long lines, long offset) {
    int result, i;
    size_t length = STR_LEN;
    rewind(data);
    fseek(data,offset,SEEK_SET);
    for(i=0; i<lines; i++) {
        result = fread(buffer[i], sizeof(char), length, data);
        if(result < length) return (i-1);
        if(i + totalReadLines == limit) return i;
    }
    return lines;
}

void parse(char *data, int floats, char returnBuffer[3][TOK_LEN]) {
    int i,j;

    for(i=0; i<TOK_LEN; i++) {
        for(j=0; j<floats; j++) {
            returnBuffer[j][i] = data[i + j*(TOK_LEN + 1)];
        }
    }
}

void printTime(struct timespec start, struct timespec end) {
    const int DAS_NANO_SECONDS_IN_SEC = 1000000000;
    long timeElapsed_s = end.tv_sec - start.tv_sec;
    long timeElapsed_n = end.tv_nsec - start.tv_nsec;
    if ( timeElapsed_n < 0 ) {
        timeElapsed_n =
            DAS_NANO_SECONDS_IN_SEC + timeElapsed_n;
        timeElapsed_s--;
    }
    printf("Time: %ld.%09ld secs \n",timeElapsed_s,timeElapsed_n);
}
