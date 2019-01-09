#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <omp.h> 

#define ARRAY_SIZE 10
#define NUM_THREADS 4

void print_array(char *msg, int *array) {
	printf("%s\n", msg);
	for (int i = 0; i < ARRAY_SIZE; i++) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

int main() {
	int matrix[ARRAY_SIZE][ARRAY_SIZE] = { 0 };
	int vector[ARRAY_SIZE] = { 0 };
	int res[ARRAY_SIZE] = { 0 };
	int i, j;
	
	omp_set_num_threads(NUM_THREADS);
	
	time_t t;  
	srand((unsigned) time(&t));
	int upper = 10; int lower = 0;
	#pragma omp parallel for shared(matrix, vector) private(i, j)
	for (i = 0; i < ARRAY_SIZE; i++) { 
		for (j = 0; j < ARRAY_SIZE; j++) {
			matrix[i][j] = (rand() % (upper - lower + 1)) + lower; 
		}
		vector[i] = (rand() % (upper - lower + 1)) + lower;
    }
	
	#pragma omp parallel for shared(matrix, vector) private(i, j)
	for (i = 0; i < ARRAY_SIZE; i++) {
		for (j = 0; j < ARRAY_SIZE; j++) {
			res[i] += matrix[i][j] * vector[j];
		}
	}
	
	print_array("The result of the multiplication", res);
	
	return 0;
}