#include "Cache.h"
#include "CacheConstants.h"
#include "CacheController.h"	
#include "CacheSet.h"
#include "vector"
#include "CacheJob.h"
#include "queue"
#include "BusRequest.h"

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
}

int Cache::getProcessorId(){
	return processorId;
}

void Cache::handleRequest(){
	if (currentJob == NULL){
		//so there are still jobs and we're not doing one right now
		if(!pendingJobs.empty()){
			currentJob = pendingJobs.front();
			pendingJobs.pop();

			if((*currentJob).isWrite()){
				//pId, bus command (BusRdX, BusRd
				busRequest = new BusRequest();



			/*
			1) see if it's a write job
				if so, we need bus access
				so make a busrequest obj and set busreqneeded to true
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


}

//from parsing the current memory job, 
//if it needs to get access to the bus, make the obj and return it here
//when the bus calls on us
BusRequest Cache::getBusRequest(){

}

/*
Read the current BusRequest that another cache issued to the bus
and parse it to see if you need to update our own local cache
*/
void Cache::snoopBusRequest(BusRequest request){

}



/*
Delete current job
Look at queue if there is another job for us to do
if there is-> handleRequest()
otherwise, maybe have a function to notify the CacheController that we're done?
*/
void Cache::busJobDone(){

}

void Cache::tick(){
}

Cache::~Cache(void){
	
}
