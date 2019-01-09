// Used src from the thread by the link:
// https://stackoverflow.com/questions/9269399/sending-blocks-of-2d-array-in-c-using-mpi/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"

#define MASTER 0
#define GLOBAL_GRID_SIZE 10
#define LOCAL_GRID_SIZE 5
#define PROC_GRID_SIZE 2 // size of process grid

int main(int argc, char **argv) {
	int global[GLOBAL_GRID_SIZE][GLOBAL_GRID_SIZE] = { 0 };
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* fill metadata */
    if (rank == MASTER) {
		global[0][0] 									= 1;
		global[0][1] 									= 1;
		global[0][LOCAL_GRID_SIZE] 						= LOCAL_GRID_SIZE + 1;
		global[0][LOCAL_GRID_SIZE + 1] 					= 1;
		global[LOCAL_GRID_SIZE][0] 						= 1;
		global[LOCAL_GRID_SIZE][1] 						= LOCAL_GRID_SIZE + 1;
		global[LOCAL_GRID_SIZE][LOCAL_GRID_SIZE] 		= LOCAL_GRID_SIZE + 1;
		global[LOCAL_GRID_SIZE][LOCAL_GRID_SIZE + 1] 	= LOCAL_GRID_SIZE + 1;
    }
	
    /* create a datatype to describe the subarrays of the global array */
	// global size
    int array_of_sizes[2] = { 
		GLOBAL_GRID_SIZE, 
		GLOBAL_GRID_SIZE 
	};
	// local size
    int arra_of_subsizes[2] = { 
		LOCAL_GRID_SIZE, 
		LOCAL_GRID_SIZE 
	};
	// where this one starts 
    int array_of_starts[2] = { 0, 0 };
    MPI_Datatype type, sendtype;
    MPI_Type_create_subarray(2, 
							 array_of_sizes, 
							 arra_of_subsizes, 
							 array_of_starts, 
							 MPI_ORDER_C, 
							 MPI_INT, 
							 &type);
    MPI_Type_create_resized(type, 
							0, 
							LOCAL_GRID_SIZE * sizeof(int), 
							&sendtype);
    MPI_Type_commit(&sendtype);

    int *sendbuf = NULL;
    if (rank == 0) {
		sendbuf = &(global[0][0]);
	}
	
    /* scatter the array to all processors */
    int sendcounts[size];
    int displs[size];

    if (rank == 0) {
        for (int i = 0; i < size; i++) {
			sendcounts[i] = 1;
		}
        int disp = 0;
        for (int i = 0; i < PROC_GRID_SIZE; i++) {
            for (int j = 0; j < PROC_GRID_SIZE; j++) {
                displs[i * PROC_GRID_SIZE + j] = disp;
                disp += 1;
            }
            disp += (LOCAL_GRID_SIZE - 1) * PROC_GRID_SIZE;
        }
    }
	
	int recvcount = GLOBAL_GRID_SIZE * GLOBAL_GRID_SIZE / size;
	int local[LOCAL_GRID_SIZE][LOCAL_GRID_SIZE] = { 0 };
    MPI_Scatterv(sendbuf, sendcounts, displs, 
				 sendtype, &(local[0][0]), recvcount,
				 MPI_INT, MASTER, MPI_COMM_WORLD);

	int x = local[0][0];
	int y = local[0][1];
	for (int i = 0; i < LOCAL_GRID_SIZE; i++) {
		for (int j = 0; j < LOCAL_GRID_SIZE; j++) {
			local[i][j] = (x + j) * (y + i);
		}
	}
	
    /* it all goes back to process 0 */
    MPI_Gatherv(&(local[0][0]), recvcount, MPI_INT,
                sendbuf, sendcounts, displs, 
				sendtype, MASTER, MPI_COMM_WORLD);
    /* or the MPI data type */
    MPI_Type_free(&sendtype);
	
    if (rank == MASTER) {
        printf("A multiplication table:\n");
        for (int i = 0; i < GLOBAL_GRID_SIZE; i++) {
            for (int j = 0; j < GLOBAL_GRID_SIZE; j++) {
				printf("%d\t", global[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}