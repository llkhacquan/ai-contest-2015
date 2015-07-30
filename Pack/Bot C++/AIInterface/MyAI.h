#pragma once
#include "Pos2D.h"
#include "BiconnectedComponents.h"
#include "SearchEngine.h"
#include "HeuristicBase.h"
#include "mydefine.h"

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

	static CMyAI* getInstance();

	vector<TMove> history;
	TPlayer we, next, first;
	int originalBoard[BOARD_SIZE];


	TBlock boardData[BOARD_SIZE];
	Pos2D p1, p2, oP1, oP2;
	AI *p_ai;
	CSearchEngine searcher;

	// MAIN 

	void newGame();

	TMove newTurn();

	TMove ourNewTurn();

	TMove enemyNewTurn();

	bool shouldEndMoveNow();

	~CMyAI();
};

