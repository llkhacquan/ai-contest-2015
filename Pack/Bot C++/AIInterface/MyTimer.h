#pragma once
#include "mydefine.h"

class CMyTimer
{
private:
	CMyTimer();
	clock_t startTime;
	static CMyTimer* instance;
public:
	~CMyTimer();
	static CMyTimer* getInstance();

	void reset();
	inline long getTimeInMs();
	bool timeUp();
};

