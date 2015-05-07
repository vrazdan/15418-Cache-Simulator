#include "CacheLine.h"
#include "CacheConstants.h"

/*
Depending upon what protocol it is (defined in constants?), line will act differently
can just use an enum for the different states (modified, shared, invalid, exlusive shared, o = lolwut
*/

unsigned long long rawAddress; //raw address that maps to me (ignoring block offset)
int setIndex; //what set i'm in
int myTag; //unique tag identifier
CacheLine::State myState;


CacheLine::CacheLine(unsigned long long address, int set, int tag)
{
	rawAddress = address;
	setIndex = set;
	myTag = tag;
	myState = invalid;
	lastUsedCycle = 0;
}

CacheLine::State CacheLine::getState(){
	return myState;
}

void CacheLine::setState(State state)
{
	myState = state;
}

unsigned long long CacheLine::getAddress(){
	return rawAddress;
}

int CacheLine::getSetIndex(){
	return setIndex;
}

int CacheLine::getTag(){
	return myTag;
}

CacheLine::~CacheLine(void)
{
}
