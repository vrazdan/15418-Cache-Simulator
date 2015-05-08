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
	isShared = false;
}

//Handle checking if the current BusRequest is completed
//And if so, getting a new one and broadcasting it to all other caches
void AtomicBusManager::tick(){

	if(inUse){
		//so the current job being executed is completed this cycle 
		if(endCycle <= constants.getCycle()){
			//tell the cache that its job is done
			(*caches.at(currentCache)).busJobDone(isShared);
			currentCache = -1;
			inUse = false;
			isShared = false;
		}
		else{
			return;
		}
	}

	int tempNextCache = -1;
	//so either not in use, or we just finished a job
	//loop for all processors starting from next 
	for(int i = 0; i < constants.getNumProcessors(); i++){
		if(((caches.at(i)) != NULL) && (*caches.at(i)).hasBusRequest()){
			//so we will now service this cache
			currentRequest = (*caches.at(i)).getBusRequest();
			tempNextCache = i;
			//printf("found a cache to service, it's cache %d \n", tempNextCache);
			break;
		}
	}

	if(tempNextCache == -1){
		//so there are no more pending requests in the system
		//printf("no one to service, leaving \n");
		inUse = false;
		return;
	}
	currentCache = tempNextCache;
	printf("now servicing cache %d on the bus at cycle %llu \n", currentCache, constants.getCycle());

	//since only get here if we got a new job
	//update the startCycle for when we just changed jobs
	startCycle = constants.getCycle();
	endCycle = startCycle + (*currentRequest).getCycleCost(); 
	inUse = true;


	//so now we have the new currentRequest and currentCache is the cache that asked for that request
	//so now we broadcast this currentRequest to all the caches other than the one who sent it
	for(int i = 0; i < constants.getNumProcessors(); i++){
		if(i != currentCache){
			Cache::SnoopResult result = (*caches.at(i)).snoopBusRequest(currentRequest);
			if(constants.getProtocol() == CacheConstants::MSI){
				{
					if (result == Cache::FLUSH_MODIFIED_TO_SHARED || result == Cache::FLUSH_MODIFIED_TO_INVALID)
					{
						endCycle += constants.getMemoryResponseCycleCost();
						(*caches[currentCache]).updateEndCycleTime(constants.getMemoryResponseCycleCost());
						//make sure the cache itself knows that it isn't finished until the proper time
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
			if(constants.getProtocol() == CacheConstants::MESI){
				if(result == Cache::FLUSH_MODIFIED_TO_INVALID){
					endCycle += constants.getMemoryResponseCycleCost();
					(*caches[currentCache]).updateEndCycleTime(constants.getMemoryResponseCycleCost());
					//so requesting cache should set the line to modified
					//this happens from a busrdx command
				}
				if(result == Cache::FLUSH_MODIFIED_TO_SHARED){
					//so this happens when there is a busrd req
					endCycle += constants.getMemoryResponseCycleCost();
					(*caches[currentCache]).updateEndCycleTime(constants.getMemoryResponseCycleCost());
					//so tell the cache that it shoudl set the line to shared
					isShared = true;
				}
				if(result == Cache::SHARED){
					isShared = true;
				}
				if(result == Cache::NONE){
					//nothing
				}
			}
		}
		//so now all caches have acknowledged the new BusRequest that was issued
	}

	//all caches ack the bus request
	//now for mesi, we tell the requesting cache if it should set the line it gets to exclusive or shared, if it was a rd req
	//if a write req, it should always go to modified


}


AtomicBusManager::~AtomicBusManager(void){
}
