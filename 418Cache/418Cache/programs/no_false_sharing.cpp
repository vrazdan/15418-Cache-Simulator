#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
using namespace std;

#define NUM_THREADS     4
#define NUM_ELEMENTS    1000

int thread_counts = 0;

size_t student_array_useless_number0[NUM_ELEMENTS];
size_t student_array_useless_number1[NUM_ELEMENTS];
size_t student_array_useless_number2[NUM_ELEMENTS];
size_t student_array_useless_number3[NUM_ELEMENTS];
// size_t student_array_useless_number4[NUM_ELEMENTS];
// size_t student_array_useless_number5[NUM_ELEMENTS];
// size_t student_array_useless_number6[NUM_ELEMENTS];
// size_t student_array_useless_number7[NUM_ELEMENTS];
// size_t student_array_useless_number8[NUM_ELEMENTS];
// size_t student_array_useless_number9[NUM_ELEMENTS];


void *access_array(void* tID)
{
    thread_counts++;
    while(thread_counts != NUM_THREADS);

   	for (size_t i = 0; i < NUM_ELEMENTS; ++i)
    {
      if ((size_t)tID == 0)
        student_array_useless_number0[i] = rand();
      if ((size_t)tID == 1)
        student_array_useless_number1[i] = rand();
      if ((size_t)tID == 2)
        student_array_useless_number2[i] = rand();
      if ((size_t)tID == 3)
        student_array_useless_number3[i] = rand();
   	}
    printf("yay\n");
    thread_counts--;
   	return (void*)0;
}

int main ()
{
   	pthread_t threads[NUM_THREADS];
   	int rc;
   	size_t i;
    srand (time(0));
   	for( i=0; i < NUM_THREADS; i++ ){
      	cout << "main() : creating thread, " << i << endl;
      	rc = pthread_create(&threads[i], NULL, 
                          access_array, (void*)i);
      	if (rc){
         	cout << "Error:unable to create thread," << rc << endl;
         	exit(-1);
      	}
   	}

    while(thread_counts != 0);

    // printf("%lu %lu %lu %lu\n", student_array[100].useless_number0, 
    //   student_array[100].useless_number1,
    //   student_array[100].useless_number2,
    //   student_array[100].useless_number3);
   	return 0;
}
