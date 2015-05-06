#pragma once
#include "string"
class CacheConstants
{
public:
	CacheConstants(void);
	int getCacheHitCycleCost();
	int getMemoryResponseCycleCost();
	int getNumProcessors();
	int getNumSets();
	int getNumLinesInSet();
	int getNumBytesInLine();
	int getNumCacheSize();
	unsigned long long getCycle();
	void tick();
	std::string getProtocol();
	int getNumBytesBits();
	int getNumSetBits();


	~CacheConstants(void);
};

