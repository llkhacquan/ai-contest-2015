#pragma once
#include "Pos2D.h"
#include "HeuristicBase.h"
class CSearchEngine
{
public:
	CSearchEngine();
	~CSearchEngine();

	enum SearchFunction
	{
		ALPHA_BETA, MTDF_ITERATIVE_DEEPENING
	};

	clock_t startTime;
	SearchFunction flag;
	TMove optimalMove(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next);
	TMove optimalMove(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int maxDepth);

	CHeuristicBase heuristic;

	int alphaBeta(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	int mtdfIterativeDeepening(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth);
	int mtdF(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int f, int depth);
};

