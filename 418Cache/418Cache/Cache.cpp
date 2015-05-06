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
For the currentJob that we have, see if the line needed for it
is in the modified state or not
*/
bool inModifiedState(){
	int* set;
	int* tag;
	decode_address((*currentJob).getAddress(), set, tag);
	/*
	random thought-
	we can tell if it's false sharing or not if the block offset isn't the same
	as like what someone else has used
	or something like that
	*/
	
	for(int i = 0; i < localCache.size(); i++){
		if((localCache[i] != NULL) && (*localCache[i]).hasLine(*tag)){
			CacheLine* theLine = (*localCache[i]).getLine(*tag);
			if((*theLine).getState() == CacheLine::modified){
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
				if(cacheConstants.getProtocol().c_str() == "MSI"){
					if(inModifiedState()){
						//so we can service this request ez
						startServiceCycle = cacheConstants.getCycle();
						jobCycleCost = cacheConstants.getCacheHitCycleCost();
						busy = true;
						haveBusRequest = false;
					}
					else{
						/*
						so here we have to make a BusRequest
						for whatever state transition we need for this job
							for MSI protocol
						set needbus to true
						(in busrequest service fxn, we then set the start cycle time
						and all the job cost time)
						*/
						haveBusRequest = true;
						busy = true; //aren't i almost always busy, unless no req?
						//busRequest = new BusRequest();

						/*
						busRequest = NULL;
						haveBusRequest = false;
						busy = false;
						startServiceCycle = 0;
						jobCycleCost = 0;
						*/






					}
				}
			}

			/*
			1) see if it's a write job
				if so,see if we need access
					if we need access make a busrequest obj and set busreqneeded to true
			2) if it's a read
				check to see if we have it already in the local cache
				in a modified / shared state
				if so, we good
			3) if invalid
				make a busrequest obj so that we can get the data
				set busreqneed to true
				*/

		}
	}
}

//return True if we have an outstanding bus request to issue, false otherwise
bool Cache::hasBusRequest(){
	return haveBusRequest;
}

//from parsing the current memory job, 
//if it needs to get access to the bus, make the obj and return it here
//when the bus calls on us
BusRequest* Cache::getBusRequest(){
	return NULL;
}

/*
Read the current BusRequest that another cache issued to the bus
and parse it to see if you need to update our own local cache
*/
void Cache::snoopBusRequest(BusRequest* request){

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
