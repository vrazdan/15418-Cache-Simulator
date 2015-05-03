#include "CacheManager.h"
#include <stdio.h>
#include <iostream>
#include "CacheConstants.h"

CacheManager::CacheManager(void)
{
}


CacheManager::~CacheManager(void)
{
}


int main(int argc, char* argv[]){
	CacheConstants* constants = new CacheConstants();
	
	/* right now assuming only argument passed in is the name of the trace 
	file that I am assuming is in the same directory that I'm in right now
	*/
	
	char* filename = argv[1];
	FILE* tracefile;
	tracefile = fopen(filename, "r");
	if(tracefile == NULL){
		printf("Error opening the tracefile, try again");
		exit(0);
	}




	delete constants;
	fclose(tracefile);
}