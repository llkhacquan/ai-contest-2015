#pragma once
#include "..\AI_Template\include/ai/defines.h"
#include "..\AI_Template\include/ai/AI.h"
#include "mydefine.h"
#include "BiconnectedComponentsOutput.h"
#include "FastPos1DDeque.h"

class CBiconnectedComponents
{
public:
	CBiconnectedComponents();
	~CBiconnectedComponents();

	// input
	TBlock oBoard[BOARD_SIZE];
	CBiconnectedComponentsOutput *output;
	// data

	bool visited[BOARD_SIZE];
	Pos1D parrent[BOARD_SIZE];
	int d[BOARD_SIZE];
	int low[BOARD_SIZE];
	int iCount;
	CFastPos1DDeque myStack;

	static int getEstimatedLength(TBlock const board[], const Pos1D &playePos, bool printInfo);
	static void biconnectedComponents(TBlock const board[], CBiconnectedComponentsOutput *output,
		const Pos1D &playerPos, TBlock *oBoard);
	void dfsVisit(const Pos1D & u);
	void createNewArea(const Pos1D &v1, const Pos1D &v2);
	void adjection(bool out[], Pos1D const &u);
};


