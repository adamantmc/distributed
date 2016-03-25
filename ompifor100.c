#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE 4
#define BUF_SIZE 100
#define STR_LEN 31
#define TOK_LEN 9

long read_new(FILE *file, char **buffer, long limit, long totalReadLines);
void parse(char *data, int floats, char returnBuffer[3][TOK_LEN]);

int myread(FILE *file, float *buffer);


int main (int argc, char* argv[]) 
{
printf ("aaaaa  1");
FILE *data;
int i, rank, size, sendcount, recvcount, source;
float *sendbuf;
sendbuf= malloc(sizeof(float) * 100*3);
data = fopen("d.txt","r");
myread(data,sendbuf);
 int sum, Tsum;
float *recvbuf;
recvbuf=malloc(sizeof(float) * 10*3);


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	printf("size= %d ",size);
	printf("rank= %d \n", rank);
source = 0;
sendcount = 10;
recvcount =10;
MPI_Scatter (&sendbuf, sendcount, MPI_FLOAT, &recvbuf, recvcount,MPI_FLOAT, source, MPI_COMM_WORLD);

//printf ("rank= %d Results: ----%f   %f   %f   ----%f   %f   %f \n", rank, recvbuf[0], recvbuf[1],recvbuf[2], recvbuf[3], recvbuf[4],recvbuf[5]);
sum = 0;
int j;
for(j=0;j<30;j=j+3){
	if(recvbuf[j]>10 && recvbuf[j]>10 && recvbuf[j]>10 && recvbuf[j]<20 && recvbuf[j]<20 && recvbuf[j]<20)
		{sum = sum +1; printf("sum=  %d" , sum);} 
}
	MPI_Reduce (&sum, &Tsum, 1, MPI_INT, MPI_SUM, source,MPI_COMM_WORLD);
if (rank == source) { printf ("Total Sum = %d \n", Tsum); } 


MPI_Finalize();
return (0); 

}



int myread(FILE *file, float *buffer)
{	int result,i;
	for(i=0; i<BUF_SIZE; i=i+3) {
	result = fscanf(file,"%f %f %f \n",&buffer[i],&buffer[i+1],&buffer[i+2]);
	printf("--%f  --%f  --%f \n",buffer[i],buffer[i+1],buffer[i+2]);}
	if(result == EOF) return (i-1);
}

