#include "MyTimer.h"

CMyTimer* CMyTimer::instance;

CMyTimer::CMyTimer()
{
	startTime = clock();
}

long CMyTimer::getTimeInMs()
{
	return clock() - startTime;
}

void CMyTimer::reset()
{
	startTime = clock();
}

CMyTimer* CMyTimer::getInstance()
{
	if (!instance)
		instance = new CMyTimer();
	return instance;
}

CMyTimer::~CMyTimer()
{
	instance = NULL;
}

bool CMyTimer::timeUp()
{
	if (getTimeInMs() > DANGER_TIME){
		DEBUG(cout << "timeUp - let quit!!!\n");
		return true;
	}
	else
		return false;
}
