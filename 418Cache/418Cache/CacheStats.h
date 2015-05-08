#pragma once
class CacheStats
{
public:
	unsigned long long numHit;
	unsigned long long numMiss;
	unsigned long long numFlush;
	unsigned long long numEvict;
	CacheStats(void);
	~CacheStats(void);
};

