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

	SearchFunction flag;
	pair<TMove, TPoint> optimalMove(TBlock board[], const TPos&_p1, const TPos&_p2, const TPlayer next, const vector<TMove> &history);
	pair<TMove, TPoint> getOptimalMoveByAB(TBlock board[], const TPos&_p1, const TPos&_p2, const TPlayer next, const vector<TMove> &history,
		int maxDepth);

	CHeuristicBase heuristic;
	clock_t startTime;
	TPoint alphaBeta(TBlock board[], const TPos&_p1, const TPos&_p2,
		const TPlayer next, vector<TMove> &history, int depth, TPoint a, TPoint b);
	TPoint alphaBetaTT(TBlock board[], const TPos&_p1, const TPos&_p2,
		const TPlayer next, vector<TMove> &history, int depth, TPoint a, TPoint b);
	TPoint mtdfIterativeDeepening(TBlock board[], const TPos&_p1, const TPos&_p2,
		const TPlayer next, vector<TMove> &history, int depth);
	TPoint mtdF(TBlock board[], const TPos&_p1, const TPos&_p2,
		const TPlayer next, vector<TMove> &history, TPoint f, int depth);
};

