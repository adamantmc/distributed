#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define lowLimit 12
#define highLimit 30
#define BUF_SIZE 5000000

long read(FILE *file, float **buffer, long limit);
int checkArgs(char **argv);
void printTime(struct timespec start, struct timespec end);

int main(int argc, char** argv) {
    float **buffer;
    double start,t_total;
    FILE *data;
    int i, openmp_threads, openmpi_processes;
    long limit, readLines, runTime, total = 0;
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

    buffer = malloc(sizeof(float *) * BUF_SIZE);
    if(buffer == NULL) {
        fprintf(stderr,"Could not allocate memory for buffer. Memory requested: %lu KBs.",BUF_SIZE * sizeof(float *));
        return 1;
    }

    if(openmp_threads > 0)  omp_set_num_threads(openmp_threads);
    else if(openmp_threads == -1) omp_set_num_threads(omp_get_max_threads());
    int sum=0;
    start = omp_get_wtime();
    while(!done) {
        readLines = read(data, buffer, limit);
        if(readLines == limit) {
            printf("Limit reached.\n");
            done = 1;
        }
       
        else if(readLines < BUF_SIZE) {
            printf("End of file reached\n");
            done = 1;
        }
	#pragma omp parallel for shared(buffer),reduction(+:sum)
	for(i=0;i<readLines;i++)
	{
	   	/*if(buffer[i][0]>=lowLimit && buffer[i][0]<=highLimit)
		{
			if(buffer[i][1]>=lowLimit && buffer[i][1]<=highLimit)
			{
				if(buffer[i][2]>=lowLimit && buffer[i][2]<=highLimit)
				{
					sum=sum+1;
				}
			}
		}*/
	
	if(buffer[i][0]>=lowLimit && buffer[i][0]<=highLimit && buffer[i][1]>=lowLimit && buffer[i][1]<=highLimit && buffer[i][2]>=lowLimit && buffer[i][2]<=highLimit )
	{
		sum=sum+1;
	}

	}
        printf("Debug: %ld %ld\n",total, readLines);
        total += readLines;
    }

t_total = omp_get_wtime() - start;
printf("Total = %lf secs and %d pleiades in order \n" , t_total,sum);

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
        buffer[i] = (float *) malloc(sizeof(float) * 3);
        result = fscanf(file,"%f %f %f \n",&buffer[i][0],&buffer[i][1],&buffer[i][2]);
        if(result == EOF) return (i-1);
        if(i == limit) return limit;
    
    }
    return BUF_SIZE;
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
