#pragma once
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
	pair<TMove, int> optimalMove(TBlock board[], const Pos1D&_p1, const Pos1D&_p2, const TPlayer next, const vector<TMove> &history);
	pair<TMove, int> getOptimalMoveByAB(TBlock board[], const Pos1D&_p1, const Pos1D&_p2, const TPlayer next, const vector<TMove> &history,
		int maxDepth);

	CHeuristicBase heuristic;
	clock_t startTime;
	int alphaBeta(TBlock board[], const Pos1D&_p1, const Pos1D&_p2,
		const TPlayer next, vector<TMove> &history, int depth, int a, int b);
	int alphaBetaTT(TBlock board[], const Pos1D&_p1, const Pos1D&_p2,
		const TPlayer next, vector<TMove> &history, int depth, int a, int b);
	int mtdfIterativeDeepening(TBlock board[], const Pos1D&_p1, const Pos1D&_p2,
		const TPlayer next, vector<TMove> &history, int depth);
	int mtdF(TBlock board[], const Pos1D&_p1, const Pos1D&_p2, 
		const TPlayer next, vector<TMove> &history, int f, int depth);
};

