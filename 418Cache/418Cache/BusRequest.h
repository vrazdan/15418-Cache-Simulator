#pragma once
class BusRequest
{
public:
	typedef enum {BusRd, BusRdX} BusMessage; 
	BusRequest(BusMessage, int, int);
	BusMessage getCommand();
	int getSet();
	int getTag();
	~BusRequest(void);
};

