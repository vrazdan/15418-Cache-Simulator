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


CacheConstants cacheConstants;
std::vector<CacheSet*> localCache; 
std::queue<CacheJob*> pendingJobs;
CacheJob* currentJob;
BusRequest* busRequest;
int processorId;
bool haveBusRequest;
bool busy;
unsigned long long startServiceCycle;
unsigned long long jobCycleCost;




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


Cache::Cache(int pId, CacheConstants consts, std::queue<CacheJob*> jobQueue)
{
	cacheConstants = consts;
	//make a vector of the CacheSet 
	localCache.resize(cacheConstants.getNumSets());
	for(int i = 0; i < cacheConstants.getNumSets(); i++){
		localCache[i] = new CacheSet(&consts);
	}
	processorId = pId;
	pendingJobs = jobQueue;
	currentJob = NULL;
	busRequest = NULL;
	haveBusRequest = false;
	busy = false;
	startServiceCycle = 0;
	jobCycleCost = 0;

}

/*
Given an address, and two int*, set the pointer values to the set number and 
the tag for the address
*/
void decode_address(unsigned long long address, int* whichSet, int* tag)
{

	int numSetBits = cacheConstants.getNumSetBits();
	int numBytesBits = cacheConstants.getNumBytesBits();
	int numTagBits = cacheConstants.getNumAddressBits() - (numSetBits + numBytesBits);

	int currSet = (address >> numBytesBits) & ((1 << numSetBits)-1);
	int currTag = (address >> (numSetBits + numBytesBits)) & ((1 << numTagBits)-1);

	*whichSet = currSet;
	*tag = currTag;

}

/*
For a given state, see if the line the current job we are working on is in that state
*/
bool lineInState(CacheLine::State state){
	int* set;
	int* tag;
	decode_address((*currentJob).getAddress(), set, tag);
	for(int i = 0; i < localCache.size(); i++){
		if((localCache[i] != NULL) && (*localCache[i]).hasLine(*tag)){
			CacheLine* theLine = (*localCache[i]).getLine(*tag);
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
		if(!pendingJobs.empty()){
			currentJob = pendingJobs.front();
			pendingJobs.pop();

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
						int* set;
						int* tag;
						decode_address((*currentJob).getAddress(), set, tag);
						//the cycle cost can be changed for different protocols and such
						busRequest = new BusRequest(BusRequest::BusRdX, *set, *tag, cacheConstants.getMemoryResponseCycleCost());
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

						/*
						BELOW CAN BE MADE INTO A SEPARATE FXN
						THAT JUST TAKES IN THE BUSREQUEST TYPE AND SETS THE BUSREQUEST PROPERLY
						CODE REUSEEEEEE
						*/
						//so we need to issue a request for the line
						haveBusRequest = true;
						busy = true;
						int* set;
						int* tag;
						decode_address((*currentJob).getAddress(), set, tag);
						//the cycle cost can be changed for different protocols and such
						busRequest = new BusRequest(BusRequest::BusRd, *set, *tag, cacheConstants.getMemoryResponseCycleCost());
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
	startServiceCycle = cacheConstants.getCycle();
	return busRequest;
}

/*
Read the current BusRequest that another cache issued to the bus
and parse it to see if you need to update our own local cache
*/
void Cache::snoopBusRequest(BusRequest* request){
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

}



/*
Delete current job
Look at queue if there is another job for us to do
if there is-> handleRequest()
otherwise, maybe have a function to notify the CacheController that we're done?
*/
void Cache::busJobDone(){

}



int Cache::getProcessorId(){
	return processorId;
}


void Cache::tick(){
}

Cache::~Cache(void){
	
}
