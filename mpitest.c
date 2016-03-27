#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

#define lowLimit 12
#define highLimit 30
#define STR_LEN 31
#define BUF_SIZE 5000000
#define TOK_LEN 9

void printTime(struct timespec start, struct timespec end);
long read(FILE *data, char **buffer, long lines, long offset);
void parse(char *data, int floats, char returnBuffer[3][TOK_LEN]);
int main(int argc, char *argv[])
{
    int rank, size, i;
    long file_size;
    long lines,readLines = 0;
	int sum=0;
    FILE *data = fopen("file.txt","r");
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for(i=0; i<size; i++) {
        if(rank == i) {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);

            fseek(data,0,SEEK_END);
            file_size = ftell(data);
            rewind(data);
			
            if((file_size/STR_LEN)%size==0)
                lines = (file_size/STR_LEN)/size;
            else
                lines = (file_size/STR_LEN)/size+1;

            char **buffer = malloc(sizeof(char *) * BUF_SIZE);

			float floats[3];
            char cords[3][TOK_LEN];

			int j; 
            for(j=0; j<BUF_SIZE; j++) buffer[j] = malloc(sizeof(char) * STR_LEN);

            readLines = read(data,buffer,lines,lines*i*STR_LEN);
			printf("Read lines: %ld\n",readLines);
			printf("File pointer: %ld\n",ftell(data));
			#pragma omp parallel for shared(buffer) private(cords), reduction(+:sum)
			  for(i=0; i<readLines; i++)
					{
						parse(buffer[i],3,cords);
						floats[0] = atof(cords[0]);
						if(floats[0]>=lowLimit && floats[0]<=highLimit) {
						    floats[1] = atof(cords[1]);
						    if(floats[1] >= lowLimit && floats[1] <= highLimit) {
						        floats[2] = atof(cords[2]);
						        if(floats[2] >= lowLimit && floats[2] <= highLimit) {
						            sum = sum + 1;
									//printf("%f  %f  %f \n ",floats[0],floats[1],floats[2]);
						        }
						    }
						}
					}
			printf("sum = %d",sum);
			/*for(j=0; j<readLines; j++) {
				printf("Rank: %d %s",i,buffer[j]);
			}*/

            for(j=0; j<BUF_SIZE; j++) free(buffer[j]);
            free(buffer);
            clock_gettime(CLOCK_MONOTONIC, &end);
            printTime(start,end);
        }
    }
    /*for(i=0; i<size; i++) {
        if(rank == i) {
            printf("%ld\n",ftell(data)/31);
        }
    }*/
    MPI_Finalize();
    return 0;
}
	
long read(FILE *data, char **buffer, long lines, long offset) {
    int result, i;
    size_t length = STR_LEN;
    fseek(data,offset,SEEK_SET);
    for(i=0; i<lines; i++) {
        result = fread(buffer[i], sizeof(char), length, data);
        if(result < length) return (i-1);
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
