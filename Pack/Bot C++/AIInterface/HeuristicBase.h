#pragma once
#include "mydefine.h"
#include "Pos2D.h"
#include "StaticFunctions.h"
#include "BiconnectedComponents.h"
class CHeuristicBase
{
private:
	static void exploreToPathLongestPath(TBlock board[], Pos2D &p, vector<TMove> &c, vector<TMove> &, int &length, int depth);

public:
	CHeuristicBase();
	~CHeuristicBase();
	int(*rateBoard)(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const TPlayer nextPlayer);
	int(*quickRateBoard)(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const TPlayer nextPlayer);

	static int pureTreeOfChamber(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const TPlayer nextPlayer);
	static int simpleRateBoard(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const TPlayer nextPlayer);
	static int voronoiRateBoard(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const TPlayer nextPlayer);

	// this method explore the board with nDepth
	static TMove getFirstMoveOfTheLongestPath(const TBlock board[], const Pos2D &p, const int nDepth = 8);
	static vector<TMove> getFirstMovesOfTheLongestPath(const TBlock board[], const Pos2D &p);

	static int getUpperLengthOfTheLongestPath(TBlock const board[], const Pos2D &playerPos, const int depth = 0);
	static int getLowerLengthOfTheLongestPath(TBlock const board[], const Pos2D &playerPos);

	static void sortMoves(vector<TMove> &moves, TBlock* board, const Pos2D &_player1, const Pos2D &_player2, const TPlayer next);
	static void sortMovesWithTT(vector<TMove> &moves, TBlock* board, const Pos2D &_player1, const Pos2D &_player2, const TPlayer next);

	static int evaluateBoard(const TBlock _board[], const Pos2D &_p1, const Pos2D &_p2, const TPlayer next, int &point, const bool goodEstimate = false);
};

