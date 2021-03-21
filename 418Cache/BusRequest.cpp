#include "BusRequest.h"


BusRequest::BusRequest(BusRequest::BusMessage busCommand, int set, int tag, int jobCycleCost, unsigned long long adr, unsigned long long ordTime){
	command = busCommand;
	lineSet = set;
	lineTag = tag;
	cycleCost = jobCycleCost;
	address = adr;
	orderingTime = ordTime;
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

unsigned long long BusRequest::getOrderingTime(){
	return orderingTime;
}

BusRequest::~BusRequest(void)
{
}
