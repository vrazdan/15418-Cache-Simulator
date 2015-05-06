#include "AtomicBusManager.h"
#include "CacheConstants.h"
#include "Cache.h"
#include "vector"
#include "CacheJob.h"
#include "BusRequest.h"


//current cache that had/has bus access
int currentCache;
//current bus request being served
BusRequest* currentRequest;
bool inUse;
//which cycle we started a job on
unsigned long long startCycle; 
CacheConstants constants;
//current job being worked on that has the bus access
CacheJob currentJob;
//list of all the caches in the system
std::vector<Cache*> caches;

AtomicBusManager::AtomicBusManager(CacheConstants consts, std::vector<Cache*> allCaches)
{
	constants = consts;
	caches = allCaches;
	currentCache = 0;
	inUse = false;
}

//Handle checking if the current BusRequest is completed
//And if so, getting a new one and broadcasting it to all other caches
void AtomicBusManager::tick(){
	
	if(inUse){
		//so the current job being executed is completed this cycle 
		//using getMemoryResponseCycleCosts as at this point only job on bus use memory
		if(startCycle + (*currentRequest).getCycleCost() >= constants.getCycle()){
			//tell the cache that its job is done
			(*caches.at(currentCache)).busJobDone();
		}
		else{
			return;
		}
	}

	int tempNextCache = -1;
	//so either not in use, or we just finished a job
	//loop for all processors starting from next 
	for(int i = currentCache + 1; (i % constants.getNumProcessors()) != currentCache; i++){
		if(((caches.at(i%(constants.getNumProcessors()))) != NULL) && (*caches.at(i%(constants.getNumProcessors()))).hasBusRequest()){
			//so we will now service this cache
			currentRequest = (*caches.at(i)).getBusRequest();
			tempNextCache = i;
			break;
		}
	}
	if(tempNextCache == -1){
		//so we didn't find anyone else who had a request, so see if currentCache can
		if((*caches.at(currentCache)).hasBusRequest()){
			currentRequest = (*caches.at(currentCache)).getBusRequest();
			tempNextCache = currentCache;
		}
	}
	currentCache = tempNextCache;
	
	if(tempNextCache == -1){
		//so there are no more pending requests in the system
		//we're done
		inUse = false;
		return;
	}
	
	//since only get here if we got a new job
	//update the startCycle for when we just changed jobs
	startCycle = constants.getCycle(); 
	inUse = true;

	//so now we have the new currentRequest and currentCache is the cache that asked for that request
	//so now we broadcast this currentRequest to all the caches other than the one who sent it
	for(int i = 0; i < constants.getNumProcessors(); i++){
		if(i != currentCache){
			/*
			TODO: we do not take into accout other caches saying if the line is dirty or shared or not
			so like, if the memory has to respond or not? but i think that's just for MESI/moesi
			*/
			(*caches.at(i)).snoopBusRequest(currentRequest);
		}
	}

	//so now all caches have acknowledged the new BusRequest that was issued
	//so we're done
}


AtomicBusManager::~AtomicBusManager(void){
}
