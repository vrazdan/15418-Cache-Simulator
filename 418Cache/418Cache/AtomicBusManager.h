#pragma once
#include "CacheConstants.h"
#include "Cache.h"
#include "vector"
class AtomicBusManager
{
public:
	AtomicBusManager(CacheConstants, std::vector<Cache*>* );
	void tick(void);
	~AtomicBusManager(void);
};

