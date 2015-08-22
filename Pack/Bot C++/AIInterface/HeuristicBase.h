#pragma once
#include "mydefine.h"
#include "StaticFunctions.h"
#include "BiconnectedComponents.h"
class CHeuristicBase
{
public:
	CHeuristicBase();
	~CHeuristicBase();
	TPoint(*rateBoard)(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next);
	TPoint(*quickRateBoard)(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next);

	TPoint rateBoardTT(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next, const vector<TMove> &history) const;
	TPoint quickRateBoardTT(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next, const vector<TMove> &history) const;

	static TPoint simpleRateBoard(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next);
	static TPoint voronoiRateBoard(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next);

	static TMove getFirstMove(const TBlock _board[], const TPos &p, const EXACT_LEVEL exact, const int currentExactLength);
	static TMove getExactFirstMove(const TBlock _board[], const TPos &p, const int currentExactLength);

	static int getLowerLengthOfTheLongestPath(TBlock const _board[], const TPos &playerPos);

	static TPoint evaluateBoardTT(const TBlock _board[], const TPos &_p1, const TPos &_p2, const TPlayer next, const vector<TMove> &history);

	static void sortMoves(vector<TMove> &moves, TBlock* _board, const TPos &_p1, const TPos &_p2, const TPlayer next, vector<TMove> &history);
};

