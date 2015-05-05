#pragma once
#include "CacheConstants.h"
#include "CacheJob.h"
#include "queue"
#include "BusRequest.h"

class Cache
{
public:
	Cache(int, CacheConstants,std::queue<CacheJob*> );
	int getProcessorId();
	void handleRequest();
	void tick();
	void busJobDone();
	bool hasBusRequest();
	BusRequest getBusRequest();
	void snoopBusRequest(BusRequest);

	 

	~Cache(void);
};

