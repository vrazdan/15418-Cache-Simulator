#include "BusRequest.h"


BusRequest::BusMessage command;
int lineSet;
int lineTag;
int cycleCost;

BusRequest::BusRequest(BusRequest::BusMessage busCommand, int set, int tag, int jobCycleCost){
	command = busCommand;
	lineSet = set;
	lineTag = tag;
	cycleCost = jobCycleCost;
}

BusRequest::BusMessage BusRequest::getCommand(){
	return command;
}

int BusRequest::getSet(){
	return lineSet;
}

int BusRequest::getTag(){
	return lineTag;
}

int BusRequest::getCycleCost(){
	return cycleCost;
}


BusRequest::~BusRequest(void)
{
}
