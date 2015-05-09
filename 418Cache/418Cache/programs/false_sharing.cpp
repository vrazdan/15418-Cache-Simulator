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

struct Student
{
  size_t useless_number0;
  size_t useless_number1;
  size_t useless_number2;
  size_t useless_number3;
  // size_t useless_number4;
  // size_t useless_number5;
  // size_t useless_number6;
  // size_t useless_number7;
  // size_t useless_number8;
  // size_t useless_number9;

};
Student student_array[NUM_ELEMENTS];

void *access_array(void* tID)
{
    thread_counts++;
    while(thread_counts != 4);

   	for (size_t i = 0; i < NUM_ELEMENTS; ++i)
    {
        if ((size_t)tID == 0)
            student_array[i].useless_number0 = rand();
        if ((size_t)tID == 1)
            student_array[i].useless_number1 = rand();
        if ((size_t)tID == 2)
            student_array[i].useless_number2 = rand();
        if ((size_t)tID == 3)
        //     student_array[i].useless_number3 = rand();
        // if ((size_t)tID == 4)
        //     student_array[i].useless_number4 = rand();
        // if ((size_t)tID == 5)
        //     student_array[i].useless_number5 = rand();
        // if ((size_t)tID == 6)
        //     student_array[i].useless_number6 = rand();
        // if ((size_t)tID == 7)
        //     student_array[i].useless_number7 = rand();
        // if ((size_t)tID == 8)
        //     student_array[i].useless_number8 = rand();
        // if ((size_t)tID == 9)
        //     student_array[i].useless_number9 = rand();

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

    printf("%lu %lu %lu %lu\n", student_array[100].useless_number0, 
      student_array[100].useless_number1,
      student_array[100].useless_number2,
      student_array[100].useless_number3);
   	return 0;
}
