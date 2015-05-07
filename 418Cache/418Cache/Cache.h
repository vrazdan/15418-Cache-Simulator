#pragma once
#include "CacheConstants.h"
#include "CacheJob.h"
#include "queue"
#include "BusRequest.h"
#include "CacheConstants.h"
#include "CacheController.h"	
#include "CacheSet.h"
#include "vector"
#include "CacheJob.h"
#include "queue"
#include "BusRequest.h"
#include "CacheLine.h"

class Cache
{
public:

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


	typedef enum {SHARED, FLUSH, NONE} SnoopResult;	 
	Cache(int, CacheConstants,std::queue<CacheJob*>* );
	int getProcessorId();
	void setPId(int);
	void handleRequest();
	void tick();
	void busJobDone();
	bool hasBusRequest();
	void decode_address(unsigned long long address, int* whichSet, int* tag);
	unsigned long long getTotalMemoryCost(int set, int tag);
	bool lineInState(CacheLine::State state);
	BusRequest* getBusRequest();
	Cache::SnoopResult snoopBusRequest(BusRequest*);
	~Cache(void);
};

