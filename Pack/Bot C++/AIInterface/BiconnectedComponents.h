#pragma once
#include "..\AI_Template\include/ai/defines.h"
#include "..\AI_Template\include/ai/AI.h"
#include "mydefine.h"
#include "BiconnectedComponentsOutput.h"
#include "BigDeque.h"

class CBC
{
public:
	CBC();
	~CBC();

	static int calculateLength(TBlock const board[], const TPos &playePos, const bool printInfo, const EXACT_LEVEL exact);
	static void calculateBCs(TBlock const board[], CBCO *output, const TPos &playerPos);
private:
	static void calculateBCs(TBlock const board[], CBCO *output, const TPos &playerPos, TBlock *oBoard);
	// input
	TBlock oBoard[BOARD_SIZE];
	CBCO *output;

	// data
	bool visited[BOARD_SIZE];
	TPos parrent[BOARD_SIZE];
	int d[BOARD_SIZE];
	int low[BOARD_SIZE];
	int iCount;
	CBigDeque myStack;

	void dfsVisit(const TPos & u);
	void createNewArea(const TPos &v1, const TPos &v2);
	void adjection(bool out[], TPos const &u) const;
};


