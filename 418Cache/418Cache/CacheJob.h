#pragma once
class CacheJob
{
public:
	CacheJob(char, unsigned long long, int);
	~CacheJob(void);
	bool isRead();
	bool isWrite();
	int getThreadId();
	unsigned long long getAddress();
};

