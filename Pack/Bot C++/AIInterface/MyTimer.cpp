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
	if (DISABLE_TIMEOUT)
		return false;
	if (getTimeInMs() > DANGER_TIME){
		return true;
	}
	else
		return false;
}
