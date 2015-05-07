#pragma once
class BusRequest
{
public:
	typedef enum {BusRd, BusRdX} BusMessage; 
	BusRequest::BusMessage command;
	int lineSet;
	int lineTag;
	int cycleCost;
	BusRequest(BusMessage, int, int, int);
	BusMessage getCommand();
	int getSet();
	int getTag();
	int getCycleCost();
	~BusRequest(void);
};

