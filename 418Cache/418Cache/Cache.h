#pragma once
#include "CacheConstants.h"
class Cache
{
public:
	Cache(int, CacheConstants);
	int getProcessorId();
	void handleRequest(char, unsigned long long, int);
	 

	~Cache(void);
};

