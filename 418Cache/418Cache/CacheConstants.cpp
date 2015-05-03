#include "CacheConstants.h"

int cacheHitCycleCost;
int memoryResponseCycleCost;
int numProcessors;

int numSets;
int numLinesInSet;
int numBytesInLine;

int numCacheSize; //= 2^sets * 2^lines * bytesPerLine

unsigned long long cycles; //total count of all cycles elapsed in simulation

//i don't care what the blockoffset actually is, because i don't care what the data actually is

//so an address is: sets+ (log base 2 of bytesPerLine)

//Intel i7 has 32k, 8 way associative, 64 byte line 
//size = # sets * #lines per set (8 in this case) * 64
//S = 2^s sets
//E = 2^e lines per set
//B = 2^b bytes per block


CacheConstants::CacheConstants(void)
{
	cacheHitCycleCost = 4;
	memoryResponseCycleCost = 100;
	numProcessors = 8; //4 core, hyperthreading
	numSets = 64;  //totalCacheSize / (numLinesInSet * (numBytesInLine));
	numLinesInSet = 8; //8 way associative
	numBytesInLine = 64; //64 bytes per line
	numCacheSize = (numSets * numLinesInSet * numBytesInLine); //~32k
	cycles = 0;
}

int CacheConstants::getCacheHitCycleCost(){
	return cacheHitCycleCost;
}
int CacheConstants::getMemoryResponseCycleCost(){
	return memoryResponseCycleCost;
}
int CacheConstants::getNumProcessors(){
	return numProcessors;
}
int CacheConstants::getNumSets(){
	return numSets;
}
int CacheConstants::getNumLinesInSet(){
	return numLinesInSet;
}
int CacheConstants::getNumBytesInLine(){
	return numBytesInLine;
}
int CacheConstants::getNumCacheSize(){
	return numCacheSize;
}
int CacheConstants::getCycle(){
	return cycles;
}

void CacheConstants::incrementCycle(){
	cycles++;
}




CacheConstants::~CacheConstants(void)
{
}
