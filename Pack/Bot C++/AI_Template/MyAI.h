#pragma once
#include "mydefine.h"
#include <vector>
#include "..\AIInterface\Pos2D.h"
#include "..\AIInterface\BiconnectedComponents.h"
#include "..\AIInterface\SearchEngine.h"
#include "..\AIInterface\HeuristicBase.h"

using namespace std;

class CMyAI
{
private:
	CMyAI();
	static CMyAI *instance;
public:
	static CMyAI* getInstance();
	AI *p_ai;
	int doneDepth;
	int iTurn;
	CSearchEngine searcher;
	int nObjectsIn5x5;

	TBlock boardData[BOARD_SIZE];
	Pos2D posPlayer1, posPlayer2, *myPos = NULL, *enemyPos = NULL;
	TPlayer we;

	// MAIN 
	void updateBoard(const int* newBoard, const Pos2D &_p1, const Pos2D &_p2, const TPlayer next, const TPlayer we);
	TMove newTurn();

	~CMyAI();
};

