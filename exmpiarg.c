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

/*allagi
ISSUES
1. limit
2.runTime
3. openmpi_processes
*/
int checkArgs(char **argv);
/* ./allagi */
void printTime(struct timespec start, struct timespec end);
long read(FILE *data, char **buffer, long lines, long offset);
void parse(char *data, int floats, char returnBuffer[3][TOK_LEN]);

int main(int argc, char *argv[])
{
    struct timespec start, end;
    int rank, size, i;
    long file_size;
    long lines,readLines = 0;
    int sum=0, Tsum = 0;

/*allagi*/
int kk;
long limit, runTime;
int openmp_threads, openmpi_processes;
FILE *data;
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
/*

FILE *data = fopen("file.txt","r");

./allagi*/

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
/*allagi
    MPI_Comm_size(MPI_COMM_WORLD, &size); */
    if(openmpi_processes ==-1)  MPI_Comm_size(MPI_COMM_WORLD, &size);
    else 
    {
	MPI_Comm_size(MPI_COMM_WORLD, &openmpi_processes);
	size = openmpi_processes;
    }
printf("There are %d processes running in this MPI program\n", size);
/* ./allagi*/


/*allagi
    omp_set_num_threads(omp_get_max_threads()); */
    if(openmp_threads > 0)  omp_set_num_threads(openmp_threads);
    else if(openmp_threads == -1) omp_set_num_threads(omp_get_max_threads());
/*./allagi*/

    MPI_Barrier(MPI_COMM_WORLD);
    clock_gettime(CLOCK_MONOTONIC,&start);

    for(i=0; i<size; i++) {
        if(rank == i) {
/*allagi
            fseek(data,0,SEEK_END);
*/
	    if(limit==-1) fseek(data,0,SEEK_END);
	    else fseek(data,0,limit*31);
/* ./allagi */
            file_size = ftell(data);
            rewind(data);

            if((file_size/STR_LEN)%size==0)
                lines = (file_size/STR_LEN)/size;
            else
                lines = (file_size/STR_LEN)/size+1;

            char **buffer = malloc(sizeof(char *) * BUF_SIZE);

            float floats[3];
            char cords[3][TOK_LEN];

            int j,k;
            for(j=0; j<BUF_SIZE; j++) buffer[j] = malloc(sizeof(char) * STR_LEN);

            readLines = read(data,buffer,lines,lines*i*STR_LEN);
            printf("Read lines: %ld\n",readLines);
            printf("File pointer: %ld\n",ftell(data));
            #pragma omp parallel for shared(buffer) private(cords)
            for(k=0; k<readLines; k++)
            {
                parse(buffer[k],3,cords);
                floats[0] = atof(cords[0]);
                if(floats[0]>=lowLimit && floats[0]<=highLimit) {
                    floats[1] = atof(cords[1]);
                    if(floats[1] >= lowLimit && floats[1] <= highLimit) {
                        floats[2] = atof(cords[2]);
                        if(floats[2] >= lowLimit && floats[2] <= highLimit) {
                            //sum = sum + 1;
                            //printf("%f  %f  %f \n ",floats[0],floats[1],floats[2]);
                        }
                    }
                }
            }
            //MPI_Reduce (&sum, &Tsum, 1, MPI_FLOAT, MPI_SUM,0,MPI_COMM_WORLD);
            //printf("sum = %d",sum);
            /*for(j=0; j<readLines; j++) {
            	printf("Rank: %d %s",i,buffer[j]);
            }*/

            for(j=0; j<BUF_SIZE; j++) free(buffer[j]);
            free(buffer);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    clock_gettime(CLOCK_MONOTONIC,&end);
    printTime(start,end);

    MPI_Finalize();
    return 0;
}

long read(FILE *data, char **buffer, long lines, long offset) {
    int result, i;
    size_t length = STR_LEN;
    fseek(data,offset,SEEK_SET);
    for(i=0; i<lines; i++) {
        result = fread(buffer[i], sizeof(char), length, data);
        if(result < length) return i;
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

/*allagi*/
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
/* ./allagi */
