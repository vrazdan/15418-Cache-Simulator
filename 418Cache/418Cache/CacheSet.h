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
};

