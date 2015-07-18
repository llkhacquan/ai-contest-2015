#pragma once
#include "Pos2D.h"
class CSearchAlgorithm
{
public:
	CSearchAlgorithm();
	~CSearchAlgorithm();

	static int ab(int board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
};

