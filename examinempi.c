#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

#define lowLimit 12
#define highLimit 30
#define STR_LEN 31
#define BUF_SIZE 5000000
#define TOK_LEN 9

int checkArgs(char **argv);
void printTime(struct timespec start, struct timespec end);
long read(FILE *data, char *buffer, long lines, long offset);
void parse(char *data, int floats, char returnBuffer[3][TOK_LEN], long line);

int main(int argc, char *argv[])
{
    struct timespec start, end;
    long file_size, lines, limit, runTime, readLines = 0, remains, offset, k;
    int sum=0, total_sum = 0, rank, size, i, openmp_threads, openmpi_processes;
    FILE *data;
    char done = 0;
    char usage[] = "Usage: examine\n\t[number of collisions ( -1 as many as in file)]\n\t[maximum run time (-1 unlimited time)]\n\t[input file]\n\t[num of openmp threads (-1 all available threads)]\n\t[num of openmpi processes (-1 all available processes)]\n";

    if(checkArgs(argv) == 1) {
        printf("%s",usage);
        return 1;
    }

    limit = atol(argv[1]);
    runTime = atol(argv[2]);
    data = fopen(argv[3],"r");
    openmp_threads = atoi(argv[4]);
    openmpi_processes = atoi(argv[5]);

    if(data == NULL) {
        printf("Could not open specified file.\n");
        return 1;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(openmp_threads > 0)  omp_set_num_threads(openmp_threads);
    else if(openmp_threads == -1) omp_set_num_threads(omp_get_max_threads());

    MPI_Barrier(MPI_COMM_WORLD);
    clock_gettime(CLOCK_MONOTONIC,&start);

    for(i=0; i<size; i++) {
        if(rank == i) {

            fseek(data,0,SEEK_END);
            file_size = ftell(data);

            if(limit != -1 && limit < file_size/STR_LEN) fseek(data,limit*31,SEEK_SET);

            file_size = ftell(data);
            rewind(data);

            lines = (file_size/STR_LEN)/size;
            offset = lines*STR_LEN*i;
            remains = (file_size/STR_LEN)%size;

            if(openmpi_processes == -1 || openmpi_processes >= size) if(rank == size-1) lines += remains;
                else if(rank == openmpi_processes-1) lines += remains;

            char *buffer = malloc(sizeof(char) * BUF_SIZE * STR_LEN);

            float floats[3];
            char cords[3][TOK_LEN];

            printf("Assigned lines: %ld\n",lines);

            while(!done) {
                readLines = read(data,buffer,lines,offset);
                printf("Read lines: %ld\n",readLines);
                printf("File pointer: %ld\n", ftell(data));
                printf("\n");

                #pragma omp parallel for shared(buffer) private(cords, floats) reduction(+:sum)
                for(k=0; k<readLines; k++)
                {
                    parse(buffer,3,cords,k);
                    floats[0] = atof(cords[0]);
                    if(floats[0]>=lowLimit && floats[0]<=highLimit) {
                        floats[1] = atof(cords[1]);
                        if(floats[1] >= lowLimit && floats[1] <= highLimit) {
                            floats[2] = atof(cords[2]);
                            if(floats[2] >= lowLimit && floats[2] <= highLimit) {
                                sum = sum + 1;
                            }
                        }
                    }
                }

                offset += readLines*STR_LEN;
                lines -= readLines;
                if(lines <= 0 || readLines == 0) done = 1;
            }

            MPI_Reduce (&sum, &total_sum, 1, MPI_FLOAT, MPI_SUM,0,MPI_COMM_WORLD);

            free(buffer);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    clock_gettime(CLOCK_MONOTONIC,&end);

    if(rank == 0) {
        printf("%d Valid Collisions\n",total_sum);
        printTime(start,end);
    }

    MPI_Finalize();
    return 0;
}

long read(FILE *data, char *buffer, long lines, long offset) {
    long result = 0;
    size_t length;
    fseek(data,offset,SEEK_SET);

    int i;

    if(lines > BUF_SIZE) lines = BUF_SIZE;
    length = STR_LEN * lines;
    result = fread(buffer, sizeof(char), length, data);

    printf("Lines: %ld Offset: %ld\n",lines,offset);
    printf("Length: %ld Result: %ld Result/STR_LEN: %ld\n",length,result,result/31);

    return result / STR_LEN;
}

void parse(char *data, int floats, char returnBuffer[3][TOK_LEN], long line) {
    int i,j, temp;
    for(i=0; i<TOK_LEN; i++) {
        temp = i + line*STR_LEN;
        for(j=0; j<floats; j++) {
            returnBuffer[j][i] = data[(temp + j*(TOK_LEN + 1))];
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

int checkArgs(char **argv) {
    if(argv[1] == NULL) {
        printf("Number of collisions not specified.\n");
        return 1;
    }

    if(argv[2] == NULL) {
        printf("Maximum run time not specified.\n");
        return 1;
    }
    if(argv[3] == NULL) {
        printf("Input file not specified.\n");
        return 1;
    }
    if(argv[4] == NULL) {
        printf("OpenMP threads not specified.\n");
        return 1;
    }

    if(argv[5] == NULL) {
        printf("OpenMPI process number not specified.\n");
        return 1;
    }

    return 0;
}

