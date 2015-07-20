#pragma once
#include "Pos2D.h"
#include "HeuristicBase.h"
class CSearchAlgorithm
{
public:
	CSearchAlgorithm();
	~CSearchAlgorithm();

	static CHeuristicBase heuristic;

	int(*searchFunction)(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);

	static int alphaBeta(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	static int negaMaxWithMemory(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	static int alphaBetaWithMemory(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	static int negaMax(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);
	static int negaScout(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int depth, int a, int b);

	static int iterative_deepening();
	static int mtdF(TBlock board[], const Pos2D&_p1, const Pos2D&_p2, TPlayer next, int f, int depth);
};

