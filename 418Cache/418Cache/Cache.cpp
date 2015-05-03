#include "Cache.h"
#include "CacheConstants.h"
#include "CacheController.h"	
#include "CacheSet.h"
#include "vector"

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
int processorId;



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

void Cache::handleRequest(char readWrite, unsigned long long address, int threadId){
	/*
	1st add instruction to queue
	*/
	

}

Cache::~Cache(void){
	for(int i = 0; i < cacheConstants.getNumSets(); i++){
		delete localCache[i];
	}

}
