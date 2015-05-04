#pragma once
#include "CacheConstants.h"
#include "CacheJob.h"


class Cache
{
public:
	Cache(int, CacheConstants);
	int getProcessorId();
	void handleRequest(CacheJob);
	 

	~Cache(void);
};

