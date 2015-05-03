#pragma once
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
	int getCycle();
	void incrementCycle();


	~CacheConstants(void);
};

