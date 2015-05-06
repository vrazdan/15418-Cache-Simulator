#include "BusRequest.h"


BusRequest::BusMessage command;
int lineSet;
int lineTag;

BusRequest::BusRequest(BusRequest::BusMessage busCommand, int set, int tag){
	command = busCommand;
	lineSet = set;
	lineTag = tag;
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



BusRequest::~BusRequest(void)
{
}
