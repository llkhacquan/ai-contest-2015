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
		ALPHA_BETA, NEGAMAX_WITH_MEMORY, NEGAMAX, NEGASCOUT, MTDF_ITERATIVE_DEEPENING
	};

	clock_t startTime;
	SearchFunction flag;
	TMove optimalMove(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next);

	CHeuristicBase heuristic;

	int alphaBeta(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	// int negaMaxWithMemory(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	int negaMaxWithMemory(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	int negaMax(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	int negaScout(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);

	int iterative_deepening(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth);
	int mtdF(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int f, int depth);
};

