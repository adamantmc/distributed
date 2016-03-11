/* 
 * File:   main.c
 * Author: aristeidis
 *
 * Created on March 5, 2016, 2:29 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * 
 */


int checkArgs(char **argv);

int main(int argc, char** argv) {
    char usage[] = "Usage: \n\t[File name to save the coordinates]\n\t[Number of coordinates that will be used]\n";

    if(checkArgs(argv) == 1) {
        printf("%s",usage);
        return 1;
    }
    FILE *f = fopen(argv[1], "w");
    if (f == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    int utime;
    long int ltime;
    int i;
    int numberCoord = atol(argv[2]);
    float cords[numberCoord][3];
    ltime = time(NULL);
    utime = (unsigned int) ltime / 2;
    srand(utime);
    for (i = 0; i < numberCoord; i++) {
        cords[i][0] = (float) 34 * rand() / (RAND_MAX - 1);
        cords[i][1] = (float) 34 * rand() / (RAND_MAX - 1);
        cords[i][2] = (float) 34 * rand() / (RAND_MAX - 1);
         fprintf(f, "%f %f %f \n", cords[i][0], cords[i][1],cords[i][2]);
    }
    fclose(f);

}
int checkArgs(char **argv) {
    if(argv[1] == NULL) {
        printf("File name to save the coordinates not specified.\n");
        return 1;
    }

    if(argv[2] == NULL) {
        printf("Number of coordinates that will be used not specified.\n");
        return 1;
    }

    return 0;
}
