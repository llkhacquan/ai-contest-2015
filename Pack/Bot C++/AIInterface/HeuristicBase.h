#pragma once
#include "..\AI_Template\mydefine.h"
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
	int(*rateBoard)(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);
	int(*quickRateBoard)(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);

	static int pureTreeOfChamber(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);
	static int pureTreeOfChamber2(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);
	static int simpleRateBoard(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);
	static int voronoiRateBoard(TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const int nextPlayer);

	// this method explore the board with nDepth
	static TMove getFirstMoveOfTheLongestPath(const TBlock board[], const Pos2D &p, const int nDepth = 8);
	static vector<TMove> getFirstMovesOfTheLongestPath(const TBlock board[], const Pos2D &p);

	static int getEstimatedLengthOfTheLongestPath(TBlock const board[], const Pos2D &playerPos);

	static void sortMoves(vector<TMove> &moves, TBlock* board, const Pos2D &_player1, const Pos2D &_player2, const TPlayer next);

	static int evaluateBoard(const TBlock _board[], const Pos2D &_player1, const Pos2D &_player2, const TPlayer nextPlayer, int &point);
};

