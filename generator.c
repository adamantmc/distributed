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



int main(int argc, char** argv) {
    FILE *f = fopen("file.txt", "w");
    if (f == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    int utime;
    long int ltime;
    int i;
    float cords[250][3];
    ltime = time(NULL);
    utime = (unsigned int) ltime / 2;
    srand(utime);
    for (i = 0; i < 250; i++) {
        cords[i][0] = (float) 34 * rand() / (RAND_MAX - 1);
        cords[i][1] = (float) 34 * rand() / (RAND_MAX - 1);
        cords[i][2] = (float) 34 * rand() / (RAND_MAX - 1);
         fprintf(f, "%f %f %f \n", cords[i][0], cords[i][1],cords[i][2]);
    }
    fclose(f);

}

