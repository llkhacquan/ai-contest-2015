#pragma once
#include "mydefine.h"
#include <vector>
#include "..\AIInterface\Pos2D.h"
#include "..\AIInterface\BiconnectedComponents.h"
#include "..\AIInterface\SearchAlgorithm.h"
#include "..\AIInterface\HeuristicBase.h"

using namespace std;

class CMyAI
{
public:
	AI *p_ai;
	static int DEPTH;
	CSearchAlgorithm searcher;

	TBlock boardData[BOARD_SIZE];
	Pos2D posPlayer1, posPlayer2, *myPos = NULL, *enemyPos = NULL;
	TPlayer we;

	// MAIN 
	void updateBoard(const int* newBoard, const Pos2D &_p1, const Pos2D &_p2, const TPlayer next, const TPlayer we);
	TMove newTurn();
	TMove optimalMove(TBlock board[], const Pos2D &_pos1, const Pos2D &_pos2, const TPlayer nextPlayer);

	CMyAI();
	~CMyAI();

};

