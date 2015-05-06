#pragma once
class BusRequest
{
public:
	typedef enum {BusRd, BusRdX} BusMessage; 
	BusRequest(BusMessage);
	~BusRequest(void);
};

