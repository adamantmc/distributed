#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

#define lowLimit 12
#define highLimit 30
#define BUF_SIZE 5000000
#define STR_LEN 31
#define TOK_LEN 9

long read(FILE *file, float **buffer, long limit);
long read_new(FILE *file, char **buffer, long limit, long totalReadLines);
int checkArgs(char **argv);
void printTime(struct timespec start, struct timespec end);
void parse(char *data, int floats, char returnBuffer[3][TOK_LEN]);

int main(int argc, char** argv) {
    char **buffer = NULL;
    double start,t_total;
    FILE *data;
    int i, j, openmp_threads, openmpi_processes;
    long limit, readLines, runTime, total = 0, sum = 0;
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
        printf("Could not open specified file.");
    }

    //Allocate memory for BUF_SIZE char pointers. Each char pointer will hold a line.
    buffer = malloc(sizeof(char *) * BUF_SIZE);
    if(buffer == NULL) {
        fprintf(stderr,"Could not allocate memory for buffer. Memory requested: %lu KBs.",BUF_SIZE * sizeof(char *));
        return 1;
    }
    //Allocate memory for each line
    for(i=0; i<BUF_SIZE; i++) {
        buffer[i] = malloc(sizeof(char) * STR_LEN);
        if(buffer[i] == NULL) {
            fprintf(stderr,"Could not allocate memory for string. Memory requested: %lu KBs.",STR_LEN*sizeof(char));
        }
    }

    //char **cords = malloc(sizeof(char *)*3);
    char cords[3][TOK_LEN];
    float floats[3];
    for(i=0; i<3; i++) buffer[i] = malloc(sizeof(char)*TOK_LEN);

    if(openmp_threads > 0)  omp_set_num_threads(openmp_threads);
    else if(openmp_threads == -1) omp_set_num_threads(omp_get_max_threads());

    start = omp_get_wtime();
    printf ("-==================rank= %d\n",done);
    while(!done) {
    printf ("-==================rank= %d\n",done);
        readLines = read_new(data, buffer, limit, total);
	printf ("-==================readlines= %ld\n",readLines);
        if(readLines != -1) total += readLines;
        printf("Lines read so far: %ld | Lines read at latest read call: %ld\n",total, readLines);
       
        if(total == limit) {
            printf("Limit reached. Read lines: %ld Total: %ld\n",readLines,total);
            done = 1;
        }

        else if(readLines < BUF_SIZE) {
            printf("End of file reached\n");
            done = 1;
        }
        char **B=NULL;
        B = malloc(sizeof(char *) * (10));
        int rank;
        int size;
        printf ("-==================rank= %d  readlines = %ld\n",openmpi_processes,readLines);
        //openMpi code
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	printf ("-==================rank= %d  readlines = %ld\n",openmpi_processes,readLines);
	printf ("rank= %d\n",rank);
	MPI_Scatter (&buffer, 10, MPI_FLOAT, &B,10,MPI_FLOAT,0, MPI_COMM_WORLD);
	printf ("rank= %d\n",rank);
        for(i=0; i<100; i++)
        {
            parse(B[i],3,cords);
            floats[0] = atof(cords[0]);
            if(floats[0]>=lowLimit && floats[0]<=highLimit) {
                floats[1] = atof(cords[1]);
                if(floats[1] >= lowLimit && floats[1] <= highLimit) {
                    floats[2] = atof(cords[2]);
                    if(floats[2] >= lowLimit && floats[2] <= highLimit) {
                        //sum = sum + 1;
                        printf("s1 = %f   s2 = %f    s3 = %f\n",floats[0],floats[1],floats[2]);
                    }
                }
            }
        }
        MPI_Finalize();
    }

    t_total = omp_get_wtime() - start;
    printf("Total = %lf secs \n" , t_total);

    fclose(data);
    return 0;
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

long read(FILE *file, float **buffer, long limit) {
    int i, result;
    for(i=0; i<BUF_SIZE; i++) {
        if(buffer[i] != NULL) free(buffer[i]);
        buffer[i] = (float *) malloc(sizeof(float) * 3);
        result = fscanf(file,"%f %f %f \n",&buffer[i][0],&buffer[i][1],&buffer[i][2]);
        if(result == EOF) return (i-1);
        if(i == limit) return limit;

    }
    return BUF_SIZE;
}

long read_new(FILE *data, char **buffer, long limit, long totalReadLines) {
	printf("==============\n");
    int result, i;
    size_t length = STR_LEN;
    for(i=0; i<BUF_SIZE; i++) {
        //result =  getline(&buffer[i],&length,data);
        result = fread(buffer[i], sizeof(char), length, data);
        if(result < length) return (i-1);
        if(i + totalReadLines == limit) return i;
    }
    return BUF_SIZE;
}

void parse(char *data, int floats, char returnBuffer[3][TOK_LEN]) {
    int i,j;
    //char buffer[floats][TOK_LEN];

    for(i=0; i<TOK_LEN; i++) {
        for(j=0; j<floats; j++) {
            returnBuffer[j][i] = data[i + j*(TOK_LEN + 1)];
        }
    }
    //for(j=0; j<floats; j++) returnBuffer[j] = buffer[j];
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
