#pragma once
#include "CacheConstants.h"
class CacheLine
{
public:
	typedef enum {invalid, shared, modified, exclusive, open} State;

	CacheLine(unsigned long long, int, int);
	unsigned long long getAddress();
	int getTag();
	int getSetIndex();
	State getState();
	void setState(State state);
	~CacheLine(void);
};

