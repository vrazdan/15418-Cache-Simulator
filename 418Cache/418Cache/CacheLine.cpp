#include "CacheLine.h"

/*
Depending upon what protocol it is (defined in constants?), line will act differently
can just use an enum for the different states (modified, shared, invalid, exlusive shared, o = lolwut
*/

enum {invalid, shared, modified, exclusive, open} state;

CacheLine::CacheLine(void)
{
}


CacheLine::~CacheLine(void)
{
}
