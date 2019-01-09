#include <stdio.h> 
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
	int proc_rank;
	
	MPI_Init(&argc, &argv); 
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	
	for (int i = 0; i < argc; i++) {
		printf("Rank - %d, arg - %s\n", proc_rank, argv[i]);
	}
	
	MPI_Finalize();
	return 0;
}