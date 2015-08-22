#pragma once
#include "BiconnectedComponents.h"
#include "SearchEngine.h"
#include "HeuristicBase.h"
#include "mydefine.h"
#include "MyTimer.h"

using namespace std;

class CMyAI
{
private:
	CMyAI();
	static CMyAI *instance;
	void printInformation();
public:
	bool inEnemyTurn = false;
	bool isCalculatingInEnemyTurn = false;
	bool firstMoveIsOver = false;
	bool activeIsolatedMode = false;
	bool useGoodEvaluation = false;
	int lastReachedDepth = 0;
	int iTurn = -1;
	bool foundAnEnd = false;

	static CMyAI* getInstance();

	vector<TMove> history;
	TPlayer we, next, first;
	int originalBoard[BOARD_SIZE];


	TBlock boardData[BOARD_SIZE];
	TPos p1, p2, oP1, oP2;
	AI *p_ai;
	CSearchEngine searcher;

	// MAIN 

	void newGame();

	TMove newTurn();

	TMove ourNewTurn();

	TMove enemyNewTurn();

	inline bool shouldEndMoveNow()
	{
		static CMyTimer* timer = CMyTimer::getInstance();
#if BOT_ACTIVE
#else
		return false;
#endif // BOT_ACTIVE
		if (timer->timeUp())
			return true;
		if (isCalculatingInEnemyTurn && !inEnemyTurn)
			return true;
		return false;
	}
	~CMyAI();
};

