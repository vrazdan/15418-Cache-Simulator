#include "Cache.h"
#include "CacheConstants.h"
#include "CacheController.h"	
#include "CacheSet.h"
#include "vector"
#include "CacheJob.h"
#include "queue"
#include "BusRequest.h"
#include "CacheLine.h"

/*
So this is the main class for handling a processors cache.
Will need to have multiple CacheSet classes
where a CacheSet is a wrapper for a list of CacheLine

Cache will have to manage not only a LRU policy for the cache,
but also manage asking for bus access, stalling, snooping
for the bus accesses, updating the state of the lines in the cache, 
and more.
*/


Cache::Cache(int pId, CacheConstants consts, std::queue<CacheJob*>* jobQueue)
{
	cacheConstants = consts;
	//make a vector of the CacheSet 
	localCache.resize(cacheConstants.getNumSets());
	for(int i = 0; i < cacheConstants.getNumSets(); i++){
		localCache[i] = new CacheSet(&consts);
	}
	processorId = pId;
	pendingJobs = *jobQueue;
	printf("number of pending jobs for cache %d (or is it %d ??) is %d \n", processorId, pId, pendingJobs.size());
	currentJob = NULL;
	busRequest = NULL;
	haveBusRequest = false;
	busy = false;
	startServiceCycle = 0;
	jobCycleCost = 0;

}

void Cache::setPId(int pid){
	processorId = pid;
}


/*
Given an address, and two int*, set the pointer values to the set number and 
the tag for the address
*/
void Cache::decode_address(unsigned long long address, int* whichSet, int* tag)
{
	int numSetBits = cacheConstants.getNumSetBits();
	int numBytesBits = cacheConstants.getNumBytesBits();
	int numTagBits = cacheConstants.getNumAddressBits() - (numSetBits + numBytesBits);

	int currSet = (address >> numBytesBits) & ((1 << numSetBits)-1);
	int currTag = (address >> (numSetBits + numBytesBits)) & ((1 << numTagBits)-1);

	*whichSet = currSet;
	*tag = currTag;

}

unsigned long long Cache::getTotalMemoryCost(int set, int tag)
{
	unsigned long long result = cacheConstants.getMemoryResponseCycleCost();
	CacheSet* currSet = localCache[set]; 
	if (!(*currSet).hasLine(tag))
	{
		if ((*currSet).isFull())
		{
			result = result*2;
		}
	}
	return result;
}

/*
For a given state, see if the line the current job we are working on is in that state
*/
bool Cache::lineInState(CacheLine::State state){
	int set = 0;
	int tag = 0;
	decode_address((*currentJob).getAddress(), &set, &tag);
	for(int i = 0; i < localCache.size(); i++){
		if((localCache[i] != NULL) && (*localCache[i]).hasLine(tag)){
			CacheLine* theLine = (*localCache[i]).getLine(tag);
			if((*theLine).getState() == state){
				return true;
			}
		}
	}
	return false;
}


/*
every tick
if we don't currently doing shit
then call handleRequest
else, jack off
*/
void Cache::handleRequest(){
	if (currentJob == NULL){
		//so there are still jobs and we're not doing one right now
		printf("inside handlerequest, the size of pending jobs is %d for cache %d \n", pendingJobs.size(), processorId);
		if(!pendingJobs.empty()){
			currentJob = pendingJobs.front();
			pendingJobs.pop();
			printf("lets make a job for cache %d \n", processorId);

			if((*currentJob).isWrite()){
				//so if in the MSI protocol
				if(cacheConstants.getProtocol() == CacheConstants::MSI){
					if(lineInState(CacheLine::modified)){
						//so we can service this request ez
						startServiceCycle = cacheConstants.getCycle();
						jobCycleCost = cacheConstants.getCacheHitCycleCost();
						busy = true;
						haveBusRequest = false;
					}
					else{
						/*
						Need to do a bus request since we don't have the line in modified
						either in shared or invalid or just not have it
						*/
						haveBusRequest = true;
						busy = true; //aren't i almost always busy, unless no req?
						//^ i guess busy depends on if i'm properly timing my stalls or not
						int set = 0;
						int tag = 0;
						decode_address((*currentJob).getAddress(), &set, &tag);
						//the cycle cost can be changed for different protocols and such
						unsigned long long memoryCost = getTotalMemoryCost(set, tag);

						busRequest = new BusRequest(BusRequest::BusRdX, set, tag,
							memoryCost);
						jobCycleCost = cacheConstants.getMemoryResponseCycleCost();
					}
				}
			}
			if((*currentJob).isRead()){
				if(cacheConstants.getProtocol() == CacheConstants::MSI){
					if(lineInState(CacheLine::modified) || lineInState(CacheLine::shared)){
						//so we have the line at least
						//so we can read fine
						haveBusRequest = false;
						busy = true;
						startServiceCycle = cacheConstants.getCycle();
						jobCycleCost = cacheConstants.getCacheHitCycleCost();
					}
					else{
						//regardless of if it's in invalid state
						//or we just don't ahve it
						//we're gonna have to read it from somewhere

						//so we need to issue a request for the line
						haveBusRequest = true;
						busy = true;
						int set = 0;
						int tag = 0;
						decode_address((*currentJob).getAddress(), &set, &tag);
						//the cycle cost can be changed for different protocols and such
						busRequest = new BusRequest(BusRequest::BusRd, set, tag,
							cacheConstants.getMemoryResponseCycleCost());
						jobCycleCost = cacheConstants.getMemoryResponseCycleCost();
					}
				}
			}
		}
	}
}

//return True if we have an outstanding bus request to issue, false otherwise
bool Cache::hasBusRequest(){
	return haveBusRequest;
}

//return the current busRequest / one that is needed
BusRequest* Cache::getBusRequest(){
	printf("cache %d got able to put out a bus request \n", processorId);
	startServiceCycle = cacheConstants.getCycle();
	return busRequest;
}

/*
Read the current BusRequest that another cache issued to the bus
and parse it to see if you need to update our own local cache
*/
Cache::SnoopResult Cache::snoopBusRequest(BusRequest* request){

	SnoopResult result = NONE;
	/*
	For write back snooping, we will have it so that 
		1) if it's a read and we share the line, then busmanager picks the highest 
				# cache to service it and we can somehow end the job early
		2) if its a read and we have in modified
				we flush to the bus and memory, so that the person who is listening 
				gets the data, but still ahve to wait the full time for memory to get it
				and make our version shared
		3) if its a read and we dont have it
				do nothing
		4) if it's a readx and we're in shared
			just set our line to invalid
		5) if readx and w're in modified
			set out to invalid
			flush our data out to the bus so it can update memory and such
	we can end up having variable length bus job accesses on how hard we try
	*/
	CacheSet* tempSet = localCache[(*request).getSet()];
	if((*tempSet).hasLine((*request).getTag())){
		//so we do have this line
		CacheLine* tempLine = (*tempSet).getLine((*request).getTag());
		if(cacheConstants.getProtocol() == CacheConstants::MSI){
			//so in the MSI protocol
			if((*request).getCommand() == BusRequest::BusRd){
				//so a bus read
				if((*tempLine).getState() == CacheLine::shared){
					//we share the line
					//so notify the busmanager we have the line
					//and the busmanager will select one cache to give the data
					//and will provide that to the requesting cache
					//and technically we don't have to wait since no memory involved
					//However, we are assuming that the bus controller will fetch
					//the data from memory anyways
					result = Cache::SHARED;
					return result;
				}//shared
				if((*tempLine).getState() == CacheLine::modified){
					//FLUSH LINE TO MEMORY
					//and set the state to Shared
					result = FLUSH;
					(*tempLine).setState(CacheLine::shared);
					return result;
				}//modified
				if((*tempLine).getState() == CacheLine::invalid){
					//We shouldn't do anything here – the line isn't even in the cache
					return result;
				}
			}//busrd
			if ((*request).getCommand() == BusRequest::BusRdX)
			{
				if((*tempLine).getState() == CacheLine::shared){
					//we share the line
					//so notify the busmanager we have the line
					//and the busmanager will select one cache to give the data
					//and will provide that to the requesting cache
					//and technically we don't have to wait since no memory involved
					//However, we are assuming that the bus controller will fetch
					//the data from memory anyways
					(*tempLine).setState(CacheLine::invalid);
					return result;
				}//shared
				if((*tempLine).getState() == CacheLine::modified){
					//FLUSH LINE TO MEMORY
					//and set the state to Shared
					result = FLUSH;
					(*tempLine).setState(CacheLine::invalid);
					return result;
				}//modified
				if((*tempLine).getState() == CacheLine::invalid){
					//We shouldn't do anything here – the line isn't even in the cache
					return result;
				}	
			}
		}//msi protocol
	}//we have the line
	return result;
}



/*
Delete current job
Look at queue if there is another job for us to do
if there is-> handleRequest()
otherwise, maybe have a function to notify the CacheController that we're done?
*/
void Cache::busJobDone(){
	printf("cache %d has just been told it has finished a job \n", processorId);


	unsigned long long jobAddr = (*currentJob).getAddress();
	int currJobSet = 0;
	int currJobTag = 0;
	decode_address(jobAddr, &currJobSet, &currJobTag);

	haveBusRequest = false;
	busy = false;
	CacheSet* currSet = localCache[currJobSet];

	//Need to tell if we need to evict a line from the set
	bool needToEvict = (*currSet).isFull() && (*currSet).hasLine(currJobTag);
	if (needToEvict)
	{
		//Evict the line
		(*currSet).evictLRULine();
	}

	if (!(*currSet).hasLine(currJobTag))
	{
		CacheLine* newLine = new CacheLine(jobAddr, currJobSet, currJobTag);
		//(*currSet).allLines().push_back(newLine);	
		(*currSet).addLine(newLine);
	}

	CacheLine* currLine = (*currSet).getLine(currJobTag); 
	(*currLine).lastUsedCycle = cacheConstants.getCycle();
	if((*currentJob).isWrite()){
		//Set the line's state to Modified
		(*currLine).setState(CacheLine::modified);
	}
	if((*currentJob).isRead()){
		//Set the line's state to Shared
		(*currLine).setState(CacheLine::shared);
	}
}



int Cache::getProcessorId(){
	return processorId;
}


void Cache::tick(){
	printf("cache %d is now in tick and num jobs is %d \n", processorId, pendingJobs.size());
	if(!busy){
		//so we're free to do a new request
		handleRequest();
	}
}

Cache::~Cache(void){
	
}
