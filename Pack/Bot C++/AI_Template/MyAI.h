#pragma once
#include "ai\defines.h"
#include "ai\AI.h"
#include "mydefine.h"
#include <vector>
#include "..\AIInterface\Pos2D.h"
#include "..\AIInterface\BiconnectedComponents.h"

using namespace std;

class CMyAI
{
public:
	AI *p_ai;
	static int DEPTH;

	int boardData[BOARD_SIZE];
	Pos2D posPlayer1, posPlayer2, *myPos = NULL, *enemyPos = NULL;
	TPlayer we;

	// MAIN 
	void updateBoard(const int* newBoard, const Pos2D &_p1, const Pos2D &_p2, const TPlayer next, const TPlayer we);
	TMove newTurn();
	static TMove optimalMove(int board[], const Pos2D &_pos1, const Pos2D &_pos2, const TPlayer nextPlayer);

	CMyAI();
	~CMyAI();

};

