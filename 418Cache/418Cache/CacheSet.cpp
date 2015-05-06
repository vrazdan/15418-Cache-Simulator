#include "CacheSet.h"
#include "vector"
#include "CacheLine.h"
#include "CacheConstants.h"

std::vector<CacheLine*> allLines;
CacheConstants* consts;

CacheSet::CacheSet(CacheConstants* constants)
{
	consts = constants;
	//set it so that the size is for the number of lines in a set
	//so don't need to allocate memory later for it
	allLines.resize((*consts).getNumLinesInSet());
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





CacheSet::~CacheSet(void)
{
}
