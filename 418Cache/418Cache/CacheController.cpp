#include "CacheController.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "CacheConstants.h"
#include "Cache.h"
#include <queue>
#include "AtomicBusManager.h"


CacheController::CacheController(void)
{
}


CacheController::~CacheController(void)
{
}




bool queuesEmpty(std::vector<std::queue<CacheJob*> > requests, int numProcessors){
	bool allEmpty = true;
	for(int i = 0; i < numProcessors; i++){
		if(!requests[i].empty()){
			allEmpty = false;
		}
	}
	return allEmpty;
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
	AtomicBusManager* bus;
	std::vector<Cache*> caches;

	//keep track of all jobs that the processors have to do
	std::vector<std::queue<CacheJob*> > outstandingRequests (numProcessors); 

	//TODO: change this into accepting file names from a Traces/ folder in the same directory
	/* right now assuming only argument passed in is the name of the trace 
	file that I am assuming is in the same directory that I'm in right now
	*/
	
	char* filename = argv[1];
	if(filename == NULL){
		printf("Error, no filename given");
		exit(0);
	}
	std::ifstream tracefile(filename);
	if(!tracefile){
		printf("Error opening the tracefile, try again");
		exit(0);
	}
	


	while(getline(tracefile, line)){
		//so while there are lines to read from the trace
		sscanf(line.c_str(), "%c %llx %u", &readWrite, &address, &threadId);
		accessProcessorId = (threadId % numProcessors);
		//so accessProcessorId is now the # of the cache that is responsible
		//for this thread

		outstandingRequests.at(accessProcessorId).push(new CacheJob(readWrite, address, threadId));
		printf("rw:%c addr:%llX threadId:%d \n", readWrite, address, threadId);
	}


	for(int i = 0; i < constants.getNumProcessors(); i++){
		printf("number of jobs cache %d SHOULD have is %d \n", i, outstandingRequests.at(i).size());
		caches.push_back(new Cache(i, constants, &outstandingRequests.at(i)));
		printf("caches[%d] pId is %d address is %x \n", i, (*caches[i]).getProcessorId(), caches[i]);
		for(int y = 0; y < i; y++){
			printf("inside the for loop, up to this point cache %d pId is %d at addr %x \n", y, (*caches[y]).getProcessorId(), caches[y]);
		}
	}

	for(std::vector<Cache*>::iterator it = caches.begin(); it != caches.end(); ++it){
		printf("%d \n",(*(*it)).getProcessorId());
	}
	printf("\n len vector is %d \n", caches.size());
	
	//so now all queues are full with the jobs they need to run
	bus = new AtomicBusManager(constants, &caches);

	while(!queuesEmpty(outstandingRequests, numProcessors)){
	
		//time must first increment for the constants
		constants.tick();
		//then call for all the caches
		for(int j = 0; j < numProcessors; j++){
			printf("trying to tick for cache %d, actually ticking cache %d \n", j, (*caches.at(j)).getProcessorId());
			(*caches.at(j)).tick();
		}
		//then call the bus manager
		(*bus).tick();

	}

	printf("yo we done ayyy");
	

	for(int i = 0; i < numProcessors; i++){
		delete caches[i];
	}
	tracefile.close();

}