#include "AtomicBusManager.h"
#include "CacheConstants.h"
#include "Cache.h"
#include "vector"
#include "CacheJob.h"
#include "BusRequest.h"


AtomicBusManager::AtomicBusManager(CacheConstants consts, std::vector<Cache*>* allCaches)
{
	constants = consts;
	caches = *allCaches;
	currentCache = 0;
	startCycle = 0;
	endCycle = 0;
	inUse = false;
}

//Handle checking if the current BusRequest is completed
//And if so, getting a new one and broadcasting it to all other caches
void AtomicBusManager::tick(){
	
	if(inUse){
		//so the current job being executed is completed this cycle 
		//using getMemoryResponseCycleCosts as at this point only job on bus use memory
		if(endCycle <= constants.getCycle()){
			//tell the cache that its job is done
			(*caches.at(currentCache)).busJobDone();
			currentCache = -1;
			inUse = false;
		}
		else{
			return;
		}
	}

	int tempNextCache = -1;
	//so either not in use, or we just finished a job
	//loop for all processors starting from next 
	printf("currentCache in atomicbusmanager is %d \n", currentCache);
	for(int i = 0; i < constants.getNumProcessors(); i++){
		if(((caches.at(i)) != NULL) && (*caches.at(i)).hasBusRequest()){
			//so we will now service this cache
			currentRequest = (*caches.at(i)).getBusRequest();
			tempNextCache = i;
			printf("found a cache to service, it's cache %d \n", tempNextCache);
			break;
		}
	}
	//stupid round robin stuff or whatever
	/*
	for(int i = currentCache + 1; (i % constants.getNumProcessors()) != currentCache; i++){
		if(((caches.at(i%(constants.getNumProcessors()))) != NULL) && (*caches.at(i%(constants.getNumProcessors()))).hasBusRequest()){
			//so we will now service this cache
			currentRequest = (*caches.at(i% constants.getNumProcessors())).getBusRequest();
			tempNextCache = (i % constants.getNumProcessors());;
			printf("found a cache to service, it's cache %d \n", tempNextCache);
			break;
		}
	}
	
	if(tempNextCache == -1){
		//so we didn't find anyone else who had a request, so see if currentCache can
		printf("didn't find anyone to service, checking current cache \n");
		if((*caches.at(currentCache)).hasBusRequest()){
			printf("current cache %d did have a request \n", currentCache);
			currentRequest = (*caches.at(currentCache)).getBusRequest();
			tempNextCache = currentCache;
		}
	}
	*/
	
	if(tempNextCache == -1){
		//so there are no more pending requests in the system
		//we're done
		printf("no one to service, leaving \n");
		inUse = false;
		return;
	}
	currentCache = tempNextCache;
	printf("current Cache now just got set to %d \n", currentCache);

	//since only get here if we got a new job
	//update the startCycle for when we just changed jobs
	startCycle = constants.getCycle();
	endCycle = startCycle + (*currentRequest).getCycleCost(); 
	inUse = true;

	//so now we have the new currentRequest and currentCache is the cache that asked for that request
	//so now we broadcast this currentRequest to all the caches other than the one who sent it
	for(int i = 0; i < constants.getNumProcessors(); i++){
		if(i != currentCache){
			/*
			TODO: we do not take into accout other caches saying if the line is dirty or shared or not
			so like, if the memory has to respond or not? but i think that's just for MESI/moesi
			*/
			Cache::SnoopResult result = (*caches.at(i)).snoopBusRequest(currentRequest);
			if(constants.getProtocol() == CacheConstants::MSI){
			{
				if (result == Cache::FLUSH)
				{
					endCycle += constants.getMemoryResponseCycleCost();
				}
				if (result == Cache::SHARED)
				{
					//Do nothing
					continue;
				}
				if (result == Cache::NONE)
				{
					//Do nothing
					continue;
				}
			}

		}
	}

	//so now all caches have acknowledged the new BusRequest that was issued
	//so we're done
	}
}


AtomicBusManager::~AtomicBusManager(void){
}
