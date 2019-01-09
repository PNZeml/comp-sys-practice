#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

#define ARRAY_SIZE 10
#define NUM_THREADS 4

int main() { 
	float sqrt_table[ARRAY_SIZE][ARRAY_SIZE] = { 0 };
	int i, j;
	
	omp_set_num_threads(NUM_THREADS);
	#pragma omp parallel for shared(sqrt_table) private(i, j)
	for (i = 0; i < ARRAY_SIZE; i++) {
		for (j = 0; j < ARRAY_SIZE; j++) {
			sqrt_table[i][j] = sqrt(i * 10 + j);
		}
	}
	
	for (i = 0; i < ARRAY_SIZE; i++) {
		for (j = 0; j < ARRAY_SIZE; j++) {
			printf("%.2f ", sqrt_table[i][j]);
		}
		printf("\n");
	}
	
	return 0;
}