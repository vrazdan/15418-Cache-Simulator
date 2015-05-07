#pragma once
#include "CacheConstants.h"
#include "CacheLine.h"
class CacheSet
{
public:
	CacheSet(CacheConstants* );
	bool hasLine(int);
	CacheLine* getLine(int);
	~CacheSet(void);
	bool isFull();
	void evictLRULine();
	void addLine(CacheLine*);
};

