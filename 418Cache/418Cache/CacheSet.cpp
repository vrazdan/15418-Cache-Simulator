#include "CacheSet.h"
#include "vector"
#include "CacheLine.h"
#include "CacheConstants.h"

std::vector<CacheLine> allLines;
CacheConstants* consts;

CacheSet::CacheSet(CacheConstants* constants)
{
	consts = constants;
	//set it so that the size is for the number of lines in a set
	//so don't need to allocate memory later for it
	allLines.resize((*consts).getNumLinesInSet());
}


CacheSet::~CacheSet(void)
{
}
