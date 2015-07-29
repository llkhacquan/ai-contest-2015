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
		ALPHA_BETA_ITERATIVE_DEEPENING, MTDF_ITERATIVE_DEEPENING
	};
	
	bool usingTT;
	SearchFunction flag;
	TMove optimalMove(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next);
	TMove optimalMove(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int maxDepth);

	CHeuristicBase heuristic;
	clock_t startTime;
	int alphaBeta(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	int alphaBetaWithTT(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	int mtdfIterativeDeepening(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth);
	int mtdF(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int f, int depth);
};

