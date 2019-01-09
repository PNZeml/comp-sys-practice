#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#define ARRAY_SIZE 20
#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

int main(int argc, char *argv[]) { 
	int size, rank;	
	int offset = 0; 
	int rows = 0;
	double matrix[ARRAY_SIZE][ARRAY_SIZE] = { 0.0 };
	double vector[ARRAY_SIZE] = { 0.0 };
	double res[ARRAY_SIZE] = { 0.0 };
			
	MPI_Init(&argc, &argv); 
	MPI_Comm_size(MPI_COMM_WORLD, &size); 
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	MPI_Status status;
		
	/* master task */ 
	if (rank == MASTER) { 
		int numOfWorkers = size - 1; 
		printf("Number of worker tasks is %d\n", numOfWorkers);
		
		time_t t;  
		srand((unsigned)time(&t));
		int lower = 0; int upper = 10;
		for (int i = 0; i < ARRAY_SIZE; i++) { 
			for (int j = 0; j < ARRAY_SIZE; j++) {
				matrix[i][j] = (rand() % (upper - lower + 1)) + lower; 
			}
			vector[i] = (rand() % (upper - lower + 1)) + lower;
		}
		
		/* send matrix data to the worker tasks */ 
		int aveRow = ARRAY_SIZE / numOfWorkers; 
		int extra = ARRAY_SIZE % numOfWorkers;
		for (int dest = 1; dest <= numOfWorkers; dest++) { 
			rows = (dest <= extra) ? aveRow + 1 : aveRow; 
			
			printf("sending %d rows to task %d\n", rows, dest);
			MPI_Send(&offset, 1, MPI_INT, dest, FROM_MASTER, 
					 MPI_COMM_WORLD);
			MPI_Send(&rows, 1, MPI_INT, dest, FROM_MASTER, 
					 MPI_COMM_WORLD);
			MPI_Send(&matrix[offset][0], rows * ARRAY_SIZE, MPI_DOUBLE, dest, FROM_MASTER, 
					 MPI_COMM_WORLD); 
			MPI_Send(&vector, ARRAY_SIZE, MPI_DOUBLE, dest, FROM_MASTER, 
					 MPI_COMM_WORLD);
			
			offset += rows;
		}
		
		/* wait for results from all worker tasks */ 
		for (int source = 1; source <= numOfWorkers; source++) {
			MPI_Recv(&offset, 1, MPI_INT, source, FROM_WORKER, 
					 MPI_COMM_WORLD, &status);
			MPI_Recv(&rows, 1, MPI_INT, source, FROM_WORKER, 
					 MPI_COMM_WORLD, &status);
			MPI_Recv(&res[offset], rows, MPI_DOUBLE, source, FROM_WORKER, 
					 MPI_COMM_WORLD, &status);
		}
		
		printf("The result vector:\n"); 
		for (int i = 0; i < ARRAY_SIZE; i++) { 
			printf("%6.2f ", res[i]);
		} 
		printf ("\n");
	} 
	
	/* worker tasks */ 
	if (rank != MASTER) { 
		MPI_Recv(&offset, 1, MPI_INT, MASTER, FROM_MASTER,
				 MPI_COMM_WORLD, &status); 
		MPI_Recv(&rows, 1, MPI_INT, MASTER, FROM_MASTER, 
				 MPI_COMM_WORLD, &status); 
		MPI_Recv(&matrix, rows * ARRAY_SIZE, MPI_DOUBLE, MASTER, FROM_MASTER, 
				 MPI_COMM_WORLD, &status); 
		MPI_Recv(&vector, ARRAY_SIZE, MPI_DOUBLE, MASTER, FROM_MASTER, 
				 MPI_COMM_WORLD, &status);
		
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < ARRAY_SIZE; j++) {
				res[i] += matrix[i][j] * vector[i + offset];
			}
		}
		
		MPI_Send(&offset, 1, MPI_INT, MASTER, FROM_WORKER, 
				 MPI_COMM_WORLD); 
		MPI_Send(&rows, 1, MPI_INT, MASTER, FROM_WORKER, 
				 MPI_COMM_WORLD); 
		MPI_Send(&res, rows, MPI_DOUBLE, MASTER, FROM_WORKER, 
				 MPI_COMM_WORLD);
	} 
	
	MPI_Finalize(); 
	return 0; 
}