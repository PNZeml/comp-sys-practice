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
	int v1[ARRAY_SIZE] = { 0 };
	int v2[ARRAY_SIZE] = { 0 };
	int res[ARRAY_SIZE] = { 0 };
	int i;
	
	omp_set_num_threads(NUM_THREADS);
	
	time_t t;  
	srand((unsigned) time(&t));
	int upper = 10; int lower = 0;
	#pragma omp parallel for shared(v1, v2) private(i)
	for (i = 0; i < ARRAY_SIZE; i++) { 
        v1[i] = (rand() % (upper - lower + 1)) + lower; 
		v2[i] = (rand() % (upper - lower + 1)) + lower; 
    }
	
	#pragma omp parallel for shared(v1, v2, res) private(i)
	for (i = 0; i < ARRAY_SIZE; i++) {
		res[i] = v1[i] + v2[i];
	}
	
	print_array("The first vector:", v1);
	print_array("The second vector:", v2);
	print_array("The result of the addition:", res);
	
	return 0;
}