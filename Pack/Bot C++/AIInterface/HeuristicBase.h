#pragma once
#include "mydefine.h"
#include "StaticFunctions.h"
#include "BiconnectedComponents.h"
class CHeuristicBase
{
public:
	static void exploreToPathLongestPath(TBlock _board[], Pos1D &p, vector<TMove> &c, vector<TMove> &, int &length, int depth);

	CHeuristicBase();
	~CHeuristicBase();
	int(*rateBoard)(TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next);
	int(*quickRateBoard)(TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next);

	int rateBoardTT(TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, const vector<TMove> &history);
	int quickRateBoardTT(TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, const vector<TMove> &history);

	static int pureTreeOfChamber(TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next);
	static int simpleRateBoard(TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next);
	static int voronoiRateBoard(TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next);

	// this method explore the _board with nDepth
	static TMove getFirstMoveOfTheLongestPath(const TBlock _board[], const Pos1D &p, const int nDepth = 8);
	static vector<TMove> getTheLongestPath(const TBlock _board[], const Pos1D &p, const int nDepth);

	static int getUpperLengthOfTheLongestPath(TBlock const _board[], const Pos1D &playerPos);
	static int getLowerLengthOfTheLongestPath(TBlock const _board[], const Pos1D &playerPos);

	static void sortMoves(vector<TMove> &moves, TBlock* _board, const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, vector<TMove> &history);
	static void sortMovesWithTT(vector<TMove> &moves, TBlock* _board, const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, vector<TMove> &history);

	static int evaluateBoard(const TBlock _board[], const Pos1D &_p1, const Pos1D &_p2, const TPlayer next, int &point);
};

