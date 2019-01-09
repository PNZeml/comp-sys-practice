#include <stdio.h> 
#include <stdlib.h>
#include <omp.h> 

#define ARRAY_SIZE 10
#define NUM_THREADS 4

int main() { 
	int array[ARRAY_SIZE] = { 0 };
	int startVal = 0;
	
	omp_set_num_threads(NUM_THREADS);
	#pragma omp parallel for
	for (int i = 0; i < ARRAY_SIZE; i++) {
		array[i] = startVal;
		startVal++;
	}
	
	for (int i = 0; i < ARRAY_SIZE; i++) {
		printf("%d ", array[i]);
	}
	printf("\n%d", startVal);

	return 0; 
}