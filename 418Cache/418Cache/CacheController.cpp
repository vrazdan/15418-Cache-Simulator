#include "CacheController.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "CacheConstants.h"
#include "Cache.h"
#include <queue>

CacheController::CacheController(void)
{
}


CacheController::~CacheController(void)
{
}


int main(int argc, char* argv[]){
	CacheConstants constants;
	//local var so don't have to do an object reference each time
	int numProcessors = constants.getNumProcessors();
	int accessProcessorId = 0;
	char readWrite = ' ';
	unsigned long long address = 0;
	unsigned int threadId = 0;
	std::string line;
	int x = constants.getNumLinesInSet();
	Cache* caches = (Cache*)malloc(constants.getNumProcessors() * sizeof(Cache));
	//keep track of all jobs that the processors have to do
	//TODO: may have to break up into smaller chunks if when running larger traces, run out of memory
	std::vector<std::queue<CacheJob>> outstandingRequests (numProcessors); 




	/* right now assuming only argument passed in is the name of the trace 
	file that I am assuming is in the same directory that I'm in right now
	*/
	
	char* filename = argv[1];
	if(filename == NULL){
		printf("Error, no filename given");
		exit(0);
	}
	std::ifstream tracefile(filename);
	if(tracefile == NULL){
		printf("Error opening the tracefile, try again");
		exit(0);
	}
	
	for(int i = 0; i < (constants).getNumProcessors(); i++){
		caches[i] = *(new Cache(i, constants));
	}
	


	while(getline(tracefile, line)){
		//so while there are lines to read from the trace
		sscanf(line.c_str(), "%c %llx %u", &readWrite, &address, &threadId);
		accessProcessorId = (threadId % numProcessors);
		//so accessProcessorId is now the # of the cache that is responsible
		//for this thread


		/*
		so we will go through the entire file
		and have a queue for each processor
		of jobs that it needs to run
			if the processor doesn't have a job already, it will star tht ejob that it has
		so then once we're done this loop of adding jobs to the queues of everyone
		then we just keep clock ticking until jobs are done
		and then once everyone is done, we're done

		if we run out of memory doing so, we can just batch process the file and such


		*/

		/*
		so fill in the queue for each processor
		send the queue to each processor as a part of the constructor
		then at the end of processing the file, call a fxn for each processor
		saying start processing

		we can check when they have all finished when all the queue sizes are 0 (but we're going to be looping anyways for cycles
		so it's all good)
		*/
		





		printf("rw:%c addr:%llX threadId:%d \n", readWrite, address, threadId);
	}




	delete caches;
	tracefile.close();

}