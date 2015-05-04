#include "Cache.h"
#include "CacheConstants.h"
#include "CacheController.h"	
#include "CacheSet.h"
#include "vector"
#include "CacheJob.h"

/*
So this is the main class for handling a processors cache.
Will need to have multiple CacheSet classes
where a CacheSet is a wrapper for a list of CacheLine

Cache will have to manage not only a LRU policy for the cache,
but also manage asking for bus access, stalling, snooping
for the bus accesses, updating the state of the lines in the cache, 
and more.
*/


CacheConstants cacheConstants;
std::vector<CacheSet*> localCache; 
CacheJob currentJob;
int processorId;

/*
so every tick call,
if not currently executing a job
see if there are job son the queue
if so, take a job off the queue
see if we can service the job without having to do a bus access
	taking into account what the current job on the bus is
if so, "service" the job

if we can't service w/o bus access
try to get access
	if we can't get access, then stall 
		(we do not support instruction reordering)
	if we can get access
		get access
		issue our command on the bus 
			will have to implement a bus command
		TODO: from here, actually talk about how to deal with timing and shit

*/




Cache::Cache(int pId, CacheConstants consts)
{
	cacheConstants = consts;
	//make a vector of the CacheSet 
	localCache.resize(cacheConstants.getNumSets());
	for(int i = 0; i < cacheConstants.getNumSets(); i++){
		localCache[i] = new CacheSet(&consts);
	}

	processorId = pId;
}

int Cache::getProcessorId(){
	return processorId;
}

void Cache::handleRequest(CacheJob job){
	/*
	1st add instruction to queue
	*/
	

}

Cache::~Cache(void){
	for(int i = 0; i < cacheConstants.getNumSets(); i++){
		delete localCache[i];
	}

}
