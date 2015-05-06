#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
using namespace std;

#define NUM_THREADS     5
#define NUM_ELEMENTS    1000

int test_array[NUM_ELEMENTS];

void *access_array(void* test_array_in)
{
   	for (int i = 0; i < 5*NUM_ELEMENTS; ++i)
    {
    	int num = rand() % NUM_ELEMENTS;
      	int value = test_array[num];
      	int othernum = rand() % NUM_ELEMENTS;
      	test_array[othernum] = i + value;
   	}
   	return (void*)0;
}

int main ()
{
   	pthread_t threads[NUM_THREADS];
   	int rc;
   	int i;
    srand (time(0));
   	for( i=0; i < NUM_THREADS; i++ ){
      	cout << "main() : creating thread, " << i << endl;
      	rc = pthread_create(&threads[i], NULL, 
                          access_array, (void*)test_array);
      	if (rc){
         	cout << "Error:unable to create thread," << rc << endl;
         	exit(-1);
      	}
   	}
   	pthread_exit(NULL);
   	printf("element: %d\n", test_array[NUM_ELEMENTS/2]);
   	free(test_array);
   	return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>


// int average;

// int size = 0;


// void *calcAvg(void *arg);
// int main(int argc, char *argv[]){
// 	int size = atoi(argv[1]);
// 	int num_threads = atoi(argv[2]);
// 	pthread_t threads[num_threads];
// 	/* initialize an array of the integers to be passed */
// 	int *test_array = (int*)malloc(size*sizeof(int));
//   	for(i = 0; i < num_threads; i++){
//   		pthread_create(&i, NULL, calcAvg, (void*)nums);

//   	}

//   	/* Thread Identifier */
//   	pthread_t avgThread;


//   	pthread_join(avgThread, NULL);
//   	printf("average = %d \n",average);
//   	free(nums);

// }
// void *calcAvg(void *arg){
//   	int *val_p = (int *) arg;
//   	int sum = 0;
//   	int i = 0;
//   	for( i = 0; i < size; i++){
// 		sum += val_p[i];
//   	}
//   	average = sum / (size);
//   	pthread_exit(0);
// }