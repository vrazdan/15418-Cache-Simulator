#include "CacheSet.h"
#include "vector"
#include "CacheLine.h"
#include "CacheConstants.h"
#include "limits.h"


/*
Container of the lines for an individual set.
Manages requests for lines and LRU eviction
*/
CacheSet::CacheSet(CacheConstants* constants)
{
	consts = constants;
}

bool CacheSet::isFull()
{
	for (int i = 0; i < allLines.size(); ++i)
	{
		if (allLines[i] == NULL || ((*allLines[i]).getState() == CacheLine::invalid))
		{
			return false;
		}
	}
	return true;
}

void CacheSet::addLine(CacheLine* line){
	allLines.push_back(line);

}

bool CacheSet::hasLine(int tag){
	for(int i = 0; i < allLines.size(); i++){
		if((allLines[i] != NULL) && (*allLines[i]).getTag() == tag){
			return true;
		}
	}
	return false;
}

//return the line with this tag
//assumes line with that tag is in the set
CacheLine* CacheSet::getLine(int tag){
	for(int i = 0; i < allLines.size(); i++){
		if((allLines[i] != NULL) && (*allLines[i]).getTag() == tag){
			return allLines[i];
		}
	}
	return NULL;
}

/*
Remove the oldest line in the set
*/
void CacheSet::evictLRULine()
{
	unsigned long long leastRecentCycle = ULLONG_MAX;
	int lineToEvict;
	for (int i = 0; i < allLines.size(); ++i)
	{
		if ((allLines[i] != NULL) && (*allLines[i]).lastUsedCycle < leastRecentCycle)
		{
			leastRecentCycle = (*allLines[i]).lastUsedCycle;
			lineToEvict = i;
		}
	}
	allLines.erase(allLines.begin() + lineToEvict);
}

CacheSet::~CacheSet(void)
{
}
